#include <glib.h>
#include <libconfig.h>
#include <string.h>

#include "../common.h"

#define CONFIG_DEFAULT 0
#define CONFIG_USER    1

void getMonitorConfigPath(const char *name, char *path)
{
  sprintf(path, "%s/.config/lwp/monitors/%s.cfg", g_get_home_dir(), name);
}

void getAppConfigPath(char *path, int type)
{
  if (type == CONFIG_DEFAULT)
    sprintf(path, "/etc/lwp.cfg");
  else
    sprintf(path, "%s/.config/lwp/lwp.cfg", g_get_home_dir());
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
  getMonitorConfigPath(name, path);

  if (!config_write_file(&cfg, path))
  {
    fprintf(stderr, "Error while writing file.\n");
    config_destroy(&cfg);
  }

  config_destroy(&cfg);
}

int loadMonitorConfig(const char *name, MonitorConfig *mc)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  char path[PATH_MAX];
  getMonitorConfigPath(name, path);

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

  return 1;
}

int loadAppConfig(AppConfig *ac)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  char path[PATH_MAX];
  getAppConfigPath(path, CONFIG_USER);

  config_init(&cfg);
  if (config_read_file(&cfg, path) == CONFIG_FALSE)
  {
    getAppConfigPath(path, CONFIG_DEFAULT);
    if (config_read_file(&cfg, path) == CONFIG_FALSE) return 0;
  }
  root = config_root_setting(&cfg);

  setting              = config_setting_get_member(root, "draw_on_rootwindow");
  ac->drawOnRootWindow = config_setting_get_int(setting);
  setting              = config_setting_get_member(root, "target_fps");
  ac->targetFps        = config_setting_get_int(setting);

  config_destroy(&cfg);

  return 1;
}

int loadWallpaperConfig(const char *dirName, WallpaperConfig *wc)
{
  char path[PATH_MAX];
  sprintf(path, "%s/wallpaper.cfg", dirName);

  config_t          cfg;
  config_setting_t *root, *setting;

  config_init(&cfg);
  if (config_read_file(&cfg, path) == CONFIG_FALSE) return 0;
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

  return 1;
}
