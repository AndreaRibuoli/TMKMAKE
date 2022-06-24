#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>

#include        "tmkbase.h"
#include        "tmkutil.h"
#include        "tmkfile.h"
#include        "tmkparse.h"
#include        "tmkdict.h"
#include        "tmkbuilt.h"
#include        "tmkoptio.h"
#include        "tmkmsghd.h"

Static  Rules_t *default_cmd    = NULL;
Static  Sufxl_t *suffix_list    = NULL;
Static  Rules_t *infer_rules    = NULL;

Static  Void    free_suffix_list ( Void );
Static  Void    dump_infer_rules ( Rules_t *rp );

Void    setup_builtin_structures ( Void ) {
  Rules_t *tp;
  Rules_t *ntp;
  Rules_t *dp;
  Rules_t *ndp;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:setup_builtin_structure(Void)\n");
#endif
  if( default_cmd != NULL ) {
    free_element( default_cmd->target );
    free_cmd( default_cmd->cmd );
    free( default_cmd );
    default_cmd = NULL;
  }
  
  if( suffix_list != NULL ) {
    free_suffix_list();
    suffix_list = NULL;
  }
  
  tp  = infer_rules;
  while( tp != NULL ) {
    dp  = (Rules_t *)tp->dependent;
    while( dp != NULL ) {
      free_element( dp->target );
      free_cmd( dp->cmd );
      ndp = dp->nxt;
      free( dp );
      dp  = ndp;
    }
    free_element( tp->target );
    ntp = tp->nxt;
    free( tp );
    tp = ntp;
  }
  infer_rules = NULL;
}

Void    read_qmaksrc_builtin ( Void ) {
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:read_qmaksrc_builtin(Void)\n");
#endif
  if( ! opt_no_built_in_rule() ) {
    if( parse_makefile( "*CURLIB/QMAKSRC(BUILTIN)",
                       PARSE_BUILTIN ) == 0 ) {
      if( parse_makefile( "*LIBL/QMAKSRC(BUILTIN)",
                         PARSE_BUILTIN ) == 0 ) {
        log_error( BUITLIN_QMAKSRC_NOT_FOUND, NULL,
                  MSG_NO_LINE_NO );
      }
    }
  }
}

Static
Boolean same_infer_type ( File_spec_t *tfs, File_spec_t *dfs ) {
  Boolean  rv;
#ifdef SRVOPT
  if( srvopt_function() ) {
    sprintf(srv_cat,"FCT:same_infer_type(%s,\n",srv_fs(tfs));
    printf("%s  %s)\n",srv_cat,srv_fs(dfs));
  }
#endif
  rv    = strcmp( tfs->type,     dfs->type     ) == 0 &&
  strcmp( tfs->seu_type, dfs->seu_type ) == 0 &&
  ( tfs->is_file
   ? strcmp( tfs->file, dfs->file ) == 0 : 1
   );
#ifdef SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:same_infer_type:%d\n",rv);
#endif
  return( rv );
  
}

Static
Boolean in_suffix_list ( File_spec_t *tfs ) {
  Boolean rc = FALSE;
  Sufxl_t *sl = suffix_list;
  
  while( sl ) {
    if( strcmp( sl->type,     tfs->type     ) == 0 &&
       strcmp( sl->seu_type, tfs->seu_type ) == 0 &&
       strcmp( sl->file,     tfs->file     ) == 0 ) {
      rc = TRUE;
      break;
    }
    sl = sl->nxt;
  }
  return( rc );
}

Static
Rules_t *in_infer_target ( File_spec_t *tfs, Rules_t *rp ) {
#ifdef SRVOPT
  if( srvopt_function() ) {
    sprintf(srv_cat,"FCT:in_infer_target(%s,",srv_fs(tfs));
    printf("%s%s);\n",srv_cat,srv_rule(rp));
  }
#endif
  while( rp ) {
    if( in_suffix_list( &rp->target->fs ) &&
       same_infer_type( &rp->target->fs, tfs ) )
      break;
    rp      = rp->nxt;
  }
#ifdef SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:in_infer_target:%s\n",srv_rule(rp));
#endif
  return( rp );
}

Static
Cmd_t   *find_inf_cmd ( File_spec_t *tfs, File_spec_t *dfs ) {
  Rules_t *rp;
  
#ifdef SRVOPT
  if( srvopt_function() ) {
    sprintf(srv_cat,"FCT:find_inf_cmd(%s,\n",srv_fs(tfs));
    printf("%s  %s)\n",srv_cat,srv_fs(dfs));
  }
#endif
  if( ( memcmp( tfs->type, FS_T_LIBFILE,
               sizeof( FS_T_LIBFILE ) ) == 0 && *tfs->extmbr == 0 ) ||
     ( memcmp( tfs->type, FS_T_TXTLIB,
              sizeof( FS_T_TXTLIB  ) ) == 0 && *tfs->extmbr == 0 ) ||
     ( memcmp( tfs->type, FS_T_BNDDIR,
              sizeof( FS_T_BNDDIR  ) ) == 0 && *tfs->extmbr == 0 ) ) {
    return( NULL );
  }
  
  if( !same_obj_base( tfs, dfs ) )
    return( NULL );
  
  if( ( rp = in_infer_target( tfs, infer_rules ) ) != NULL ) {
#if 1
    Rules_t *dp;
    
    if( ( dp = in_infer_target( &rp->target->fs,
                               (Rules_t *)rp->dependent ) ) != NULL ) {
#ifdef SRVOPT
      if( srvopt_fctrtn() )
        printf("RTN:find_inf_cmd:%s\n",srv_cmd(dp->cmd));
#endif
      return( dp->cmd );
    }
    
#else
    Rules_t *dp     = (Rules_t *)rp->dependent;
    
    while( dp ) {
      if( same_infer_type( &dp->target->fs, dfs ) ) {
#ifdef SRVOPT
        if( srvopt_fctrtn() )
          printf("RTN:find_inf_cmd:%s\n",srv_cmd(dp->cmd));
#endif
        return( dp->cmd );
      }
      dp      = dp->nxt;
    }
#endif
  }
  return( NULL );
}

Static
Rules_t *find_valid_infer_rule ( File_spec_t *tfs, Int16 line ) {
  Rules_t       *rp;
  File_spec_t   fs;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:find_valid_infer_rule(%s)\n",srv_fs(tfs));
#endif
  if( ( memcmp( tfs->type, FS_T_LIBFILE,
               sizeof( FS_T_LIBFILE ) ) == 0 && *tfs->extmbr == 0 ) ||
     ( memcmp( tfs->type, FS_T_TXTLIB,
              sizeof( FS_T_TXTLIB  ) ) == 0 && *tfs->extmbr == 0 ) ||
     ( memcmp( tfs->type, FS_T_BNDDIR,
              sizeof( FS_T_BNDDIR  ) ) == 0 && *tfs->extmbr == 0 ) ) {
    return( NULL );
  }
  
  
  if( ( rp = in_infer_target( tfs, infer_rules ) ) != NULL ) {
    rp      = (Rules_t *) rp->dependent;
    while( rp ) {
      fs = *tfs;
      strcpy( fs.type, rp->target->fs.type );
      strcpy( fs.seu_type, rp->target->fs.seu_type );
      fs.is_file = rp->target->fs.is_file;
      
      if( ( memcmp( tfs->type, FS_T_LIBFILE,
                   sizeof( FS_T_LIBFILE ) ) == 0 ) ||
         ( memcmp( tfs->type, FS_T_TXTLIB,
                  sizeof( FS_T_TXTLIB  ) ) == 0 ) ||
         ( memcmp( tfs->type, FS_T_BNDDIR,
                  sizeof( FS_T_BNDDIR  ) ) == 0 )) {
        if( fs.is_file ) {
          strcpy( fs.file, rp->target->fs.file );
          if( obj_exists ( &fs, line ) )
            break;
          
          rp      = rp->nxt;
        }
        else
          break;
      }
      
      else {
        if( tfs->is_file ) {
          if( fs.is_file ) {
            strcpy( fs.file, rp->target->fs.file );
          }
          else {
            strcpy( fs.file, fs.extmbr );
            fs.extmbr[0] = 0;
          }
        }
        else {
          if( fs.is_file ) {
            strcpy( fs.extmbr, fs.file );
            strcpy( fs.file, rp->target->fs.file );
          }
        }
        
        if( obj_exists ( &fs, line ) )
          break;
        
        rp      = rp->nxt;
      }
    }
  }
#ifdef SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:find_valid_infer_rule:%s\n",srv_rule(rp));
#endif
  return( rp );
}


Static
Void    update_file_nm ( File_spec_t *tfs, File_spec_t *dfs ) {
  Char  *fromp;
  
#ifdef SRVOPT
  if( srvopt_function() ) {
    sprintf(srv_cat,"FCT:update_file_nm(%s,\n",srv_fs(tfs));
    printf("%s  %s)\n",srv_cat,srv_fs(dfs));
  }
#endif
  fromp = dfs->is_file ||
  strcmp( dfs->type, FS_T_LIBFILE ) == 0 ||
  strcmp( dfs->type, FS_T_TXTLIB  ) == 0 ||
  strcmp( dfs->type, FS_T_BNDDIR  ) == 0
  ? dfs->extmbr : dfs->file;
  
  strcpy( tfs->is_file ||
         strcmp( tfs->type, FS_T_LIBFILE ) == 0 ||
         strcmp( tfs->type, FS_T_TXTLIB  ) == 0 ||
         strcmp( dfs->type, FS_T_BNDDIR  ) == 0
         ? tfs->extmbr : tfs->file, fromp );
}

Void    apply_inference_rules ( Void ) {
  Rules_t *make_rule;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:applied_inference_rules(Void)\n");
#endif
  make_rule       = get_first_rule();
  while( make_rule != NULL ) {
    Element_t       *dp;
    Element_t       *pdp;
    
    if( make_rule->cmd == NULL ) {
      File_spec_t     *fp = &(make_rule->target->fs);
      
      pdp     = NULL;
      dp      = make_rule->dependent;
      while( dp != NULL ) {
        Cmd_t   *inf_cmd;
        
        if( ( inf_cmd = find_inf_cmd( fp, &dp->fs ) )
           != NULL ) {
          make_rule->cmd  = inf_cmd;
          make_rule->implicit_rule = TRUE;
          if( pdp != NULL ) {
            pdp->nxt        = dp->nxt;
            dp->nxt         = make_rule->dependent;
            make_rule->dependent = dp;
          }
          break;
        }
        pdp     = dp;
        dp      = dp->nxt;
      }
      if( dp == NULL ) {
        Rules_t         *inf_rule;
        Element_t       *dep;
        
        if( ( inf_rule = find_valid_infer_rule( fp,
                                               make_rule->line ) ) != NULL ) {
          make_rule->cmd  = inf_rule->cmd;
          dep = ( Element_t * )alloc_buf(
                                         sizeof( Element_t ) +
                                         strlen( inf_rule->target->fs.is_file
                                                ? inf_rule->target->fs.extmbr
                                                : inf_rule->target->fs.file ) +
                                         strlen( make_rule->target->name ) + 1,
                                         "apply_inference_rules()" );
          
          dep->maked      = FALSE;
          dep->nxt        = make_rule->dependent;
          dep->fs         = inf_rule->target->fs;
          strcpy( dep->fs.lib, fp->lib );
          update_file_nm( &dep->fs, fp );
          strcpy( dep->name, obj_full_name( &dep->fs ) );
          
          make_rule->dependent= dep;
          make_rule->implicit_rule = TRUE;
        }
      }
    }
    dp = make_rule->dependent;
    while( dp != NULL ) {
      File_spec_t     *fp = &(dp->fs);
      Rules_t         *inf_rule;
      Element_t       *tep;
      Element_t       *dep;
      if( ! in_rule_list ( dp->name ) &&
         ( ( inf_rule = find_valid_infer_rule( &dp->fs,
                                              make_rule->line ) ) != NULL ) ) {
        tep = ( Element_t * )alloc_buf(
                                       sizeof( Element_t ) + strlen( dp->name ),
                                       "apply_inference_rules()" );
        
        strcpy( tep->name, dp->name );
        tep->fs         = dp->fs;
        tep->nxt        = NULL;
        tep->maked      = FALSE;
        dep = ( Element_t * )alloc_buf(
                                       sizeof( Element_t ) +
                                       strlen( inf_rule->target->fs.file ) +
                                       strlen( dp->name ) + 1,
                                       "apply_inference_rules()" );
        
        dep->fs         = inf_rule->target->fs;
        strcpy( dep->fs.lib, dp->fs.lib );
        update_file_nm( &dep->fs, fp );
        
        strcpy( dep->name, obj_full_name( &dep->fs ) );
        dep->nxt        = NULL;
        dep->maked      = FALSE;
        
        add_inf_rule_to_rule_tail( tep, dep, inf_rule->cmd );
      }
      else {
        if( default_cmd != NULL ) {
          tep = ( Element_t * )alloc_buf(
                                         sizeof( Element_t ) +
                                         strlen( dp->name ),
                                         "apply_inference_rules()" );
          
          strcpy( tep->name, dp->name );
          tep->fs         = dp->fs;
          tep->nxt        = NULL;
          tep->maked      = FALSE;
          
          add_inf_rule_to_rule_tail( tep, NULL,
                                    default_cmd->cmd );
        }
      }
      dp      = dp->nxt;
    }
    make_rule       = get_next_rule();
  }
}

Static
Boolean add_rule_sort_suffix ( Rules_t **h, Rules_t *rp ) {
  Sufxl_t *slp    = suffix_list;
  Sufxl_t *cslp;
  Rules_t *hp     = *h;
  Rules_t *curp;
  Rules_t *prvp;
  
#ifdef SRVOPT
  if( srvopt_function() ) {
    sprintf(srv_cat,"FCT:add_rule_sort_suffix(%s,",
            srv_rule(*h));
    printf("%s%s)\n",srv_cat,srv_rule(rp));
  }
#endif
  rp->nxt = NULL;
  if( hp == NULL ) {
    *h      = rp;
    return( TRUE );
  }
  while( slp ) {
    if( strcmp( slp->type,     rp->target->fs.type     ) == 0 &&
       strcmp( slp->file,     rp->target->fs.file     ) == 0 &&
       strcmp( slp->seu_type, rp->target->fs.seu_type ) == 0 ) {
      break;
    }
    slp     = slp->nxt;
  }
  
  if( slp == NULL ) {
    while( hp->nxt )
      hp      = hp->nxt;
    hp->nxt = rp;
  } else {
    prvp    = NULL;
    curp    = *h;
    cslp    = suffix_list;
    
    while( curp && cslp && cslp != slp ) {
      if( strcmp( cslp->type,
                 curp->target->fs.type     ) == 0 &&
         strcmp( cslp->file,
                curp->target->fs.file     ) == 0 &&
         strcmp( cslp->seu_type,
                curp->target->fs.seu_type ) == 0 ) {
        prvp    = curp;
        curp    = curp->nxt;
      }
      cslp    = cslp->nxt;
    }
    if( prvp == NULL ) {
      rp->nxt   = *h;
      *h        = rp;
    }
    else {
      rp->nxt   = prvp->nxt;
      prvp->nxt = rp;
    }
  }
  
  return( TRUE );
}

Void    update_inference_rules ( Char *dp, Cmd_t *cp, Int16 line ) {
  File_spec_t     tfs;
  File_spec_t     dfs;
  File_spec_t     *fsp;
  Rules_t         *trp;
  Rules_t         *drp;
  Element_t       *ep;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:update_inference_rules(\"%s\",%s,%d)\n",
           dp?dp:"NULL",srv_cmd(cp),line );
#endif
  if( strncmp( dp, ".DEFAULT", sizeof( ".DEFAULT" ) - 1 ) == 0 ) {
    if( default_cmd == NULL ) {
      default_cmd = (Rules_t *)alloc_buf( sizeof( Rules_t ),
                                         "update_inference_rules()" );
      default_cmd->target = (Element_t *)alloc_buf(sizeof(Element_t), "update_inference_rules()" );
    }
    
    default_cmd->nxt        = NULL;
    default_cmd->dependent  = NULL;
    default_cmd->op         = OP_ERROR;
    default_cmd->recursive  =
    default_cmd->implicit_rule=  FALSE;
    default_cmd->line       = line;
    default_cmd->cmd        = cp;
    
    ep                      = default_cmd->target;
    ep->nxt                 = NULL;
    ep->maked               = FALSE;
    ep->name[0]             = 0;
    
    fsp                     = &ep->fs;
    fsp->lib[0]             =
    fsp->file[0]            =
    fsp->extmbr[0]          =
    fsp->seu_type[0]        =
    fsp->type[0]            = 0;
    fsp->is_file            = FALSE;
    fsp->obj_type           = '*';
    
    return;
  }
  
  if( !parse_inference_rule( dp, &dfs, &tfs ) ) {
    log_error( INV_INF_RULE, NULL, line );
    exit( TMK_EXIT_FAILURE );
  }
  
  if( ( trp = in_infer_target( &tfs, infer_rules ) ) == NULL ) {
    trp            = (Rules_t *)alloc_buf( sizeof( Rules_t ),
                                          "update_inference_rules()" );
    trp->target    = (Element_t *)alloc_buf( sizeof( Element_t ),
                                            "update_inference_rules()" );
    trp->cmd             = NULL;
    trp->dependent       = NULL;
    trp->op              = OP_ERROR;
    trp->line            = -1;
    trp->recursive       =
    trp->implicit_rule   =  FALSE;
    
    trp->target->nxt     = NULL;
    trp->target->maked   = FALSE;
    trp->target->name[0] = 0;
    trp->target->fs      = tfs;
    
    add_rule_sort_suffix( &infer_rules, trp );
  }
  if( ( drp = in_infer_target( &dfs, (Rules_t *)trp->dependent ) )
     == NULL ) {
    drp            = (Rules_t *)alloc_buf( sizeof( Rules_t ),
                                          "update_inference_rules()" );
    drp->target    = (Element_t *)alloc_buf( sizeof( Element_t ),
                                            "update_inference_rules()" );
    drp->cmd             = NULL;
    drp->dependent       = NULL;
    drp->op              = OP_ERROR;
    drp->line            = -1;
    drp->recursive       =
    drp->implicit_rule   =  FALSE;
    
    drp->target->nxt     = NULL;
    drp->target->maked   = FALSE;
    drp->target->name[0] = 0;
    drp->target->fs      = dfs;
    
    add_rule_sort_suffix( (Rules_t **)&trp->dependent, drp );
  }
  drp->line       = line;
  if( drp->cmd != NULL ) {
    free_cmd( drp->cmd );
  }
  drp->cmd        = cp;
}

Static
Void    sort_infer_rules ( Void ) {
  Rules_t *tp     = infer_rules;
  Rules_t *ntp;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:sort_infer_rules(Void)\n");
#endif
  infer_rules     = NULL;
  while( tp ) {
    Rules_t *dp     = (Rules_t *)tp->dependent;
    Rules_t *ndp;
    
    tp->dependent   = NULL;
    while( dp ) {
      ndp     = dp->nxt;
      add_rule_sort_suffix( (Rules_t **)&tp->dependent, dp );
      dp      = ndp;
    }
    ntp     = tp->nxt;
    add_rule_sort_suffix( &infer_rules, tp );
    tp      = ntp;
  }
}

Static
Void    free_suffix_list ( Void ) {
  Sufxl_t *slp    = suffix_list;
  Sufxl_t *snlp   = NULL;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:free_suffix_list(Void)\n");
#endif
  while( slp != NULL ) {
    snlp    = slp->nxt;
    free( slp );
    slp     = snlp;
  }
  suffix_list     = NULL;
}

Void    update_suffix ( Char *sp, Int32 line_no ) {
  Sufxl_t *slp    = suffix_list;
  Sufxl_t *snlp   = NULL;
  File_spec_t     fs;
  
#ifdef SRVOPT
  if( srvopt_function() )
    printf("FCT:update_suffix(\"%s\",%d)\n",sp, line_no);
#endif
  if( sp == NULL || *sp == 0 ) {
    free_suffix_list();
  }
  else {
    while( slp && slp->nxt != NULL )
      slp     = slp->nxt;
    
    sp      = skip_white_spaces( sp );
    while( *sp ) {
      if( *sp++ == '.' ) {
        if( ( sp = parse_file_ext( sp, &fs ) ) != NULL ) {
          snlp    = (Sufxl_t *)alloc_buf(
                                         sizeof( Sufxl_t ), "Update_suffix()" );
          snlp->obj_type = '*';
          strcpy( snlp->type,     fs.type );
          strcpy( snlp->seu_type, fs.seu_type );
          strcpy( snlp->file,     fs.file );
          if( slp == NULL ) {
            suffix_list     = snlp;
          }
          else {
            slp->nxt        = snlp;
          }
          snlp->nxt       = NULL;
          slp             = snlp;
        }
        else {
          log_error( INV_SUFFIX_SPEC, NULL, line_no );
          break;
        }
      }
      else {
        log_error( INV_SUFFIX_SPEC, NULL, line_no );
        break;
      }
      sp      = skip_white_spaces( sp );
    }
  }
  sort_infer_rules();
}

Static
Void    dump_infer_rules ( Rules_t *rp ) {
  Boolean      dbg;
  File_spec_t  *fs;
  Cmd_t        *cp;
  
  if( ! ( dbg = opt_debug() ) ) {
    return;
  }
  while( rp ) {
    Rules_t *dp     = (Rules_t *)rp->dependent;
    while( dp ) {
      reset_buf( &t1 );
      fs = &dp->target->fs;
      sprintf( txtbuf, ".%s<%s%s%s>", fs->file, fs->type,
              *fs->seu_type ? "," : "",
              fs->seu_type );
      append_buf( &t1, txtbuf );
      fs = &rp->target->fs;
      sprintf( txtbuf, ".%s<%s%s%s>", fs->file, fs->type,
              *fs->seu_type ? "," : "",
              fs->seu_type );
      append_buf( &t1, txtbuf );
      append_buf( &t1, ":" );
      log_dbg( t1.bp );
      
      cp      = dp->cmd;
      while( cp ) {
        sprintf( txtbuf, "      %s", cp->cmd_txt );
        log_dbg( txtbuf );
        cp      = cp->nxt;
      }
      if( dbg ) {
        log_dbg("-------------------------------------"
                "------------------------------------");
      }
      else {
        log_dbg( "" );
      }
      
      dp      = dp->nxt;
    }
    rp      = rp->nxt;
  }
}

Void    dump_builtins ( Void ) {
  Boolean      dbg;
  Rules_t      *rp;
  Cmd_t        *cp;
  File_spec_t  *fs;
  Sufxl_t      *slp    = suffix_list;
  
  if( ! ( dbg = opt_debug() ) ) {
    return;
  }
  
  log_dbg("*****  Built-ins / Inference rules definitions  "
          "*************************");
  
  reset_buf( &t1 );
  append_buf( &t1, ".SUFFIXES:" );
  
  while( slp ) {
    sprintf( txtbuf, " .%s<%s%s%s>", slp->file, slp->type,
            *slp->seu_type ? "," : "",
            slp->seu_type );
    append_buf( &t1, txtbuf );
    slp     = slp->nxt;
  }
  log_dbg( t1.bp );
  log_dbg( "" );
  
  if( default_cmd ) {
    if( dbg )
      log_dbg( "==========================================="
              "==============================");
    
    log_dbg( ".DEFAULT:" );
    cp      = default_cmd->cmd;
    while( cp ) {
      sprintf( txtbuf, "      %s", cp->cmd_txt );
      log_dbg( txtbuf );
      cp      = cp->nxt;
    }
    log_dbg( "" );
  }
  
  if( dbg )
    log_dbg( "============================================"
            "=============================");
  
  dump_infer_rules ( infer_rules );
  
  if( dbg ) {
    log_dbg("*********************************************"
            "**********************");
  }
}
