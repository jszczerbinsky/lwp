#include <glib.h>
#include <libconfig.h>

#include "../common.h"

void getMonitorConfigPath(const char *name, char *path)
{
  sprintf(path, "%s/.config/lwp/monitors/%s.cfg", g_get_home_dir(), name);
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
