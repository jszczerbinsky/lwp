#include <glib.h>
#include <libconfig.h>
#include <string.h>

#include "../common.h"

#define CONFIG_DEFAULT 0
#define CONFIG_USER    1

static void generateEmptyMonitorConfig(MonitorConfig *mc)
{
  sprintf(mc->wlpName, "");
  mc->wlpBounds.x = 0;
  mc->wlpBounds.y = 0;
  mc->wlpBounds.w = 1920;
  mc->wlpBounds.h = 1080;
}

void saveMonitorConfig(const char *name, MonitorConfig *mc)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  config_init(&cfg);
  root = config_root_setting(&cfg);

  setting = config_setting_add(root, "wlpName", CONFIG_TYPE_STRING);
  config_setting_set_string(setting, mc->wlpName);

  setting = config_setting_add(root, "x", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.x);
  setting = config_setting_add(root, "y", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.y);
  setting = config_setting_add(root, "w", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.w);
  setting = config_setting_add(root, "h", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.h);

  char path[PATH_MAX];
  getMonitorCfgPath(path, name);

  if (!config_write_file(&cfg, path))
  {
    fprintf(stderr, "Error while writing file.\n");
    config_destroy(&cfg);
  }

  config_destroy(&cfg);
}

int loadMonitorConfig(const char *name, MonitorConfig *mc)
{
  mc->loaded = 0;

  config_t          cfg;
  config_setting_t *root, *setting;

  char path[PATH_MAX];
  getMonitorCfgPath(path, name);

  if (!g_file_test(path, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_EXISTS))
  {
    generateEmptyMonitorConfig(mc);
    saveMonitorConfig(name, mc);
    return 1;
  }

  config_init(&cfg);
  if (config_read_file(&cfg, path) == CONFIG_FALSE) return 0;
  root = config_root_setting(&cfg);

  setting = config_setting_get_member(root, "wlpName");
  strcpy(mc->wlpName, config_setting_get_string(setting));

  setting         = config_setting_get_member(root, "x");
  mc->wlpBounds.x = config_setting_get_int(setting);
  setting         = config_setting_get_member(root, "y");
  mc->wlpBounds.y = config_setting_get_int(setting);
  setting         = config_setting_get_member(root, "w");
  mc->wlpBounds.w = config_setting_get_int(setting);
  setting         = config_setting_get_member(root, "h");
  mc->wlpBounds.h = config_setting_get_int(setting);

  config_destroy(&cfg);

  mc->loaded = 1;
  return 1;
}

static void useDefaultAppCfg(AppConfig *ac)
{
  ac->targetFps = 60;
  strcpy(ac->renderQuality, "best");
}

void saveAppConfig(AppConfig *ac)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  config_init(&cfg);
  root = config_root_setting(&cfg);

  setting = config_setting_add(root, "target_fps", CONFIG_TYPE_INT);
  config_setting_set_int(setting, ac->targetFps);
  setting = config_setting_add(root, "render_quality", CONFIG_TYPE_STRING);
  config_setting_set_string(setting, ac->renderQuality);

  char path[PATH_MAX];
  getAppCfgPath(path);

  if (!config_write_file(&cfg, path))
  {
    fprintf(stderr, "Error while writing file.\n");
    config_destroy(&cfg);
  }

  config_destroy(&cfg);
}

int loadAppConfig(AppConfig *ac)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  char path[PATH_MAX];
  getAppCfgPath(path);

  config_init(&cfg);
  if (config_read_file(&cfg, path) == CONFIG_FALSE)
  {
    useDefaultAppCfg(ac);
    return 1;
  }
  root = config_root_setting(&cfg);

  setting       = config_setting_get_member(root, "target_fps");
  ac->targetFps = config_setting_get_int(setting);
  setting       = config_setting_get_member(root, "render_quality");
  strcpy(ac->renderQuality, config_setting_get_string(setting));

  config_destroy(&cfg);

  return 1;
}

int loadWallpaperConfig(const char *dirPath, WallpaperConfig *wc)
{
  wc->loaded = 0;

  config_t          cfg;
  config_setting_t *root, *setting;

  config_init(&cfg);

  char path[PATH_MAX];
  getWlpCfgPath(path, dirPath);

  if (config_read_file(&cfg, path) == CONFIG_FALSE)
  {
    getWlpCfgPath(path, dirPath);
    if (config_read_file(&cfg, path) == CONFIG_FALSE) return 0;
  }
  root = config_root_setting(&cfg);

  setting         = config_setting_get_member(root, "count");
  wc->layersCount = config_setting_get_int(setting);
  setting         = config_setting_get_member(root, "repeat_x");
  wc->repeatX     = config_setting_get_int(setting);
  setting         = config_setting_get_member(root, "repeat_y");
  wc->repeatY     = config_setting_get_int(setting);

  wc->layerConfigs = malloc(wc->layersCount * sizeof(LayerConfig));

  setting    = config_setting_get_member(root, "movement_x");
  float movX = config_setting_get_float(setting);
  setting    = config_setting_get_member(root, "movement_y");
  float movY = config_setting_get_float(setting);

  for (int i = 0; i < wc->layersCount; i++)
  {
    LayerConfig *lc = wc->layerConfigs + i;

    lc->sensitivityX = movX * i;
    lc->sensitivityY = movY * i;
  }

  config_destroy(&cfg);

  wc->loaded = 1;

  return 1;
}
