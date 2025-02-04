#include "libxml/xmlstring.h"
#include "main.h"
#include <libxml/xmlreader.h>
#include <stdlib.h>

static void config_error_handler(void* data, const char* msg, ...) {
	printlog(LOG_ERROR, msg);
}

static void config_warning_handler(void* data, const char* msg, ...) {
	printlog(LOG_ERROR, msg);
}

static void parse_texture(WlpInstance* inst, const char* dir_path,
						  xmlNodePtr texture_node) {
	xmlChar* name = xmlGetProp(texture_node, BAD_CAST "name");
	xmlChar* filename = xmlGetProp(texture_node, BAD_CAST "filename");

	if (name && filename) {
		char path[PATH_MAX];
		sprintf(path, "%s%s%s%s%s", dir_path, DIR_SEP, "assets", DIR_SEP,
				filename);
		tex_load(inst, (char*)name, path);
		printlog(LOG_INFO, "Loading texture %s from %s", name, path);
	}

	if (filename) {
		xmlFree(filename);
	}
	if (name) {
		xmlFree(name);
	}
}

static void parse_font(WlpInstance* inst, const char* dir_path,
					   xmlNodePtr font_node) {
	xmlChar* name = xmlGetProp(font_node, BAD_CAST "name");
	xmlChar* filename = xmlGetProp(font_node, BAD_CAST "filename");
	xmlChar* sizestr = xmlGetProp(font_node, BAD_CAST "size");

	float ptsize = 18;
	if (sizestr) {
		ptsize = atoi((char*)sizestr);
	}

	if (name && filename) {
		char path[PATH_MAX];
		sprintf(path, "%s%s%s%s%s", dir_path, DIR_SEP, "assets", DIR_SEP,
				filename);
		printlog(LOG_INFO, "Loading font %s from %s", name, path);

		font_load(inst, (char*)name, ptsize, path);
	}

	if (sizestr) {
		xmlFree(sizestr);
	}
	if (filename) {
		xmlFree(filename);
	}
	if (name) {
		xmlFree(name);
	}
}

static void parse_assets(WlpInstance* inst, const char* dir_path,
						 xmlNodePtr assets_node) {
	xmlNodePtr child = assets_node->children;

	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "texture") == 0) {
			parse_texture(inst, dir_path, child);
		} else if (xmlStrcmp(child->name, BAD_CAST "font") == 0) {
			parse_font(inst, dir_path, child);
		}
		child = child->next;
	}
}

static void parse_behaviours(WlpInstance* inst, Layer* layer,
							 xmlNodePtr behaviours_node) {
	xmlChar* prop;

	xmlNodePtr child = behaviours_node->children;

	while (child) {
		float farg = 0;
		int	  iarg = 0;

		int behid = parsebehaviour((char*)child->name);

		prop = xmlGetProp(child, BAD_CAST "farg");
		if (prop) {
			farg = atof((char*)prop);
			xmlFree(prop);
		}
		prop = xmlGetProp(child, BAD_CAST "iarg");
		if (prop) {
			iarg = atoi((char*)prop);
			xmlFree(prop);
		}

		if (behid != BEHAVIOUR_INVALID) {
			layer_addbehaviour(layer, behid, farg, iarg);
		}

		child = child->next;
	}
}

static void parse_content(WlpInstance* inst, Layer* layer,
						  xmlNodePtr content_node) {
	xmlNodePtr child = content_node->children;

	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "textContent") == 0) {
			layer_setcontent(layer, CONTENT_TEXT);

			xmlChar* fontname = xmlGetProp(child, BAD_CAST "font");
			xmlChar* text = xmlGetProp(child, BAD_CAST "text");

			if (fontname) {
				Font* font = font_find(inst, (char*)fontname);
				if (font) {
					layer_text_setfont(layer, font);
				}

				xmlFree(fontname);
			}
			if (text) {
				layer_text_settext(layer, (char*)text);
				xmlFree(text);
			}

		} else if (xmlStrcmp(child->name, BAD_CAST "imageContent") == 0) {
			layer_setcontent(layer, CONTENT_IMG);

			xmlChar* texname = xmlGetProp(child, BAD_CAST "texture");

			if (texname) {
				Tex* tex = tex_find(inst, (char*)texname);
				if (tex) {
					layer_img_settex(layer, tex, 1);
				}
				xmlFree(texname);
			}
		}

		child = child->next;
	}
}

static void parse_layer(WlpInstance* inst, xmlNodePtr layer_node) {
	xmlChar* prop;

	Layer* layer = layer_spawnempty(inst);

	prop = xmlGetProp(layer_node, BAD_CAST "scalex");
	if (prop) {
		layer->scale.w = atof((char*)prop);
		xmlFree(prop);
	}
	prop = xmlGetProp(layer_node, BAD_CAST "scaley");
	if (prop) {
		layer->scale.h = atof((char*)prop);
		xmlFree(prop);
	}
	prop = xmlGetProp(layer_node, BAD_CAST "posx");
	if (prop) {
		layer->bounds.x = atof((char*)prop);
		xmlFree(prop);
	}
	prop = xmlGetProp(layer_node, BAD_CAST "posy");
	if (prop) {
		layer->bounds.y = atof((char*)prop);
		xmlFree(prop);
	}
	prop = xmlGetProp(layer_node, BAD_CAST "anchor");
	if (prop) {
		int anchor = parseanchor((char*)prop);
		if (anchor != ANCHOR_INVALID) {
			layer->anchor = anchor;
		}
		xmlFree(prop);
	}

	xmlNodePtr child = layer_node->children;

	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "content") == 0) {
			parse_content(inst, layer, child);
		} else if (xmlStrcmp(child->name, BAD_CAST "behaviours") == 0) {
			parse_behaviours(inst, layer, child);
		}

		child = child->next;
	}
}

static void parse_layers(WlpInstance* inst, xmlNodePtr layers_node) {
	xmlNodePtr child = layers_node->children;

	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "layer") == 0) {
			parse_layer(inst, child);
		}
		child = child->next;
	}
}

static void parse_wallpaper(WlpInstance* inst, const char* dir_path,
							xmlNodePtr wallpaper_node) {
	xmlNodePtr child = wallpaper_node->children;

	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "assets") == 0) {
			parse_assets(inst, dir_path, child);
		} else if (xmlStrcmp(child->name, BAD_CAST "layers") == 0) {
			parse_layers(inst, child);
		}

		child = child->next;
	}
}

void instance_load(WlpInstance* inst, const char* dir_path) {
	char config_path[PATH_MAX] =
		"/home/cziken/.config/lwp/testwlp/wallpaper.xml";
	char schema_path[PATH_MAX] =
		"/home/cziken/.config/lwp/testwlp/wallpaper.xsd";

	xmlDocPtr doc = xmlReadFile(config_path, NULL, 0);
	if (doc == NULL) {
		printf("Failed to load XML\n");
	}

	xmlSchemaParserCtxtPtr schema_ctxt = xmlSchemaNewParserCtxt(schema_path);
	xmlSchemaPtr		   schema = xmlSchemaParse(schema_ctxt);

	xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
	xmlSchemaSetValidErrors(valid_ctxt, config_error_handler,
							config_warning_handler, NULL);

	int ret = xmlSchemaValidateDoc(valid_ctxt, doc);
	if (ret == 0) {
		printf("Valid XML\n");

		xmlNodePtr root = xmlDocGetRootElement(doc);
		parse_wallpaper(inst, dir_path, root);
	} else {
		printf("Invalid XML\n");
	}

	xmlSchemaFreeValidCtxt(valid_ctxt);
	xmlSchemaFree(schema);
	xmlFreeDoc(doc);
}
