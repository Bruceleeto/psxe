#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "common.h"

#include "../psx/log.h"

static const char* g_version_text =
#ifdef _WIN32
    "psxe.exe (" STR(OS_INFO) ") " STR(REP_VERSION) "-" STR(REP_COMMIT_HASH) "\n"
#elif __linux__
    "psxe (" STR(OS_INFO) ") " STR(REP_VERSION) "-" STR(REP_COMMIT_HASH) "\n"
#else
    "psxe (" STR(OS_INFO) ") " STR(REP_VERSION) "-" STR(REP_COMMIT_HASH) "\n"
#endif
    "\nPSXE - A simple, fast and portable Sony PlayStation emulator.\n\n"
    "Copyright (C) 2023 Allkern (Lisandro Alarcon)\n"
    "This is free software; see the source for copying conditions.  There is NO\n"
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";

static const char* g_models_text =
    "Available console models:\n"
    "\"scph1000\" (SCPH-1000) [NTSC-J]\n"
    "\"scph1001\" (SCPH-1001) [NTSC-U/C]\n"
    "\"scph1002\" (SCPH-1002) [PAL]\n"
    "\"scph3000\" (SCPH-3000) [NTSC-J]\n"
    "\"scph3500\" (SCPH-3500) [NTSC-J]\n"
    "\"scph5000\" (SCPH-5000) [NTSC-U/C]\n"
    "\"scph5501\" (SCPH-5501) [NTSC-U/C]\n"
    "\"scph5500\" (SCPH-5500) [NTSC-J]\n"
    "\"scph5502\" (SCPH-5502) [PAL]\n"
    "\"scph5552\" (SCPH-5552) [PAL]\n"
    "\"scph7000\" (SCPH-7000) [NTSC-J]\n"
    "\"scph7001\" (SCPH-7001) [NTSC-U/C]\n"
    "\"scph7002\" (SCPH-7002) [PAL]\n"
    "\"scph7003\" (SCPH-7003) [NTSC-J]\n"
    "\"scph7501\" (SCPH-7501) [NTSC]\n"
    "\"scph7502\" (SCPH-7502) [PAL]\n"
    "\"scph9002\" (SCPH-9002) [PAL]\n"
    "\"scph100\" (SCPH-100) [NTSC-J]\n"
    "\"scph101\" (SCPH-101) [NTSC-U/C]\n"
    "\"scph102a\" (SCPH-102A) [PAL]\n"
    "\"scph102b\" (SCPH-102B) [PAL]\n"
    "\"scph102c\" (SCPH-102C) [?]\n";

static const char* g_regions_text =
    "Available region options: \"ntsc\", \"pal\", \"auto\"\n";

static const char* g_desc_text =
    "\nPlease report any bugs to <https://github.com/allkern/psxe/issues>\n";

psxe_config_t* psxe_cfg_create(void) {
    return (psxe_config_t*)malloc(sizeof(psxe_config_t));
}

void psxe_cfg_init(psxe_config_t* cfg) {
    memset(cfg, 0, sizeof(psxe_config_t));
}

void psxe_cfg_destroy(psxe_config_t* cfg) {
    free(cfg);
}

void psxe_cfg_load_defaults(psxe_config_t* cfg) {
    cfg->bios = "assets/bios.bin";
    cfg->bios_search = "assets";
    cfg->exe = NULL;
    cfg->help_model = 0;
    cfg->help_region = 0;
    cfg->model = "scph1001";
    cfg->scale = 3;
    cfg->psxe_version = STR(REP_VERSION);
    cfg->region = "ntsc";
    cfg->settings_path = NULL;
    cfg->use_args = 0;
    cfg->version = 0;
    cfg->log_level = LOG_FATAL;
    cfg->quiet = 0;
    cfg->cd_path = "assets/disc.cue";
    cfg->exp_path = NULL;
}

void psxe_cfg_load(psxe_config_t* cfg, int argc, const char* argv[]) {
    log_set_level(LOG_INFO);

    int version = 0;
    int help_model = 0;
    int help_region = 0;
    int log_level = 0;
    int quiet = 0;
    int scale = 0;
    const char* bios = NULL;
    const char* model = NULL;
    const char* exe = NULL;
    const char* region = NULL;
    const char* cd_path = NULL;
    const char* exp_path = NULL;

    static const char *const usages[] = {
        "psxe [options] path-to-cdrom",
        NULL,
    };

    struct argparse_option options[] = {
        OPT_BOOLEAN ('h', "help"          , NULL           , "Display this information", argparse_help_cb, 0, 0),
        OPT_BOOLEAN (0  , "help-model"    , &help_model    , "Display available console models", NULL, 0, 0),
        OPT_BOOLEAN (0  , "help-region"   , &help_region   , "Display available region options", NULL, 0, 0),
        OPT_BOOLEAN ('v', "version"       , &version       , "Display version and build information", NULL, 0, 0),
        OPT_GROUP("Basic options"),
        OPT_STRING  ('b', "bios"          , &bios          , "Specify a BIOS file", NULL, 0, 0),
        OPT_STRING  ('e', "exp-rom"       , &exp_path      , "Specify an expansion ROM file"),
        OPT_INTEGER ('L', "log-level"     , &log_level     , "Set log level"),
        OPT_STRING  ('M', "model"         , &model         , "Specify console model (SPCH-XXXX)", NULL, 0, 0),
        OPT_STRING  ('r', "region"        , &region        , "Specify console region"),
        OPT_INTEGER ('s', "scale"         , &scale         , "Display scaling factor", NULL, 0, 0),
        OPT_BOOLEAN ('q', "quiet"         , &quiet         , "Silence all logs (ignores -L)"),
        OPT_STRING  ('x', "exe"           , &exe           , "Launch a PS-X EXE file"),
        OPT_STRING  (0  , "cdrom"         , &cd_path       , "Specify a CDROM image"),
        OPT_END()
    };

    struct argparse argparse;

    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, NULL, g_desc_text);

    argc = argparse_parse(&argparse, argc, argv);

    if (help_model) {
        printf("%s\n", g_models_text);
        exit(0);
    }

    if (help_region) {
        printf("%s\n", g_regions_text);
        exit(0);
    }

    if (version) {
        printf("%s\n", g_version_text);
        exit(0);
    }

    log_set_quiet(quiet);

    if (argc) {
        if (argc > 1) {
            log_error("Unrecognized parameter \'%s\'", argv[1]);
            exit(1);
        }

        cd_path = argv[0];
    }

    if (cd_path)     cfg->cd_path    = cd_path;
    if (log_level)   cfg->log_level  = log_level - 1;
    if (bios)        cfg->bios       = bios;
    if (model)       cfg->model      = model;
    if (exe)         cfg->exe        = exe;
    if (region)      cfg->region     = region;
    if (exp_path)    cfg->exp_path   = exp_path;
    if (scale)       cfg->scale      = scale;
}

char* psxe_cfg_get_bios_path(psxe_config_t* cfg) {
    return NULL;
}
