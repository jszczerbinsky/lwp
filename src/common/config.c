#include <libconfig.h>

#include "../common.h"

void saveMonitorConfig(MonitorConfig *mc)
{
  config_t          cfg;
  config_setting_t *root, *setting;

  config_init(&cfg);
  root = config_root_setting(&cfg);

  setting = config_setting_add(root, "wlpPath", CONFIG_TYPE_STRING);
  config_setting_set_string(setting, mc->wlpPath);

  setting = config_setting_add(root, "x", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.x);
  setting = config_setting_add(root, "y", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.y);
  setting = config_setting_add(root, "w", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.w);
  setting = config_setting_add(root, "h", CONFIG_TYPE_INT);
  config_setting_set_int(setting, mc->wlpBounds.h);

  if (!config_write_file(&cfg, "test.cfg"))
  {
    fprintf(stderr, "Error while writing file.\n");
    config_destroy(&cfg);
  }

  config_destroy(&cfg);
}
