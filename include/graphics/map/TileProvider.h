#pragma once

#include <string>
#include <tuple>
#include <vector>

class TileProvider
{
  public:
    TileProvider() = default;

    // get the URL for a tile
    static std::string url(const char *filename);
    static std::string url(int z, int x, int y);

    // get all URL templates
    static const std::vector<std::string> templates(void);

    // get all URL providers (separated by newline)
    static const std::string providers(void);

    // add a custom URL template if not exist
    static int addTemplate(const std::string &name, const std::string &tmpl);

    // select/activate a template by index
    static void selectTemplate(int idx);

    // get the selected template index
    static int selectedTemplate(void);

  private:
    // list of <map name, url>
    static std::vector<std::tuple<std::string, std::string>> urlTemplates;
};
