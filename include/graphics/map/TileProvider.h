#pragma once

#include <string>
#include <vector>

class TileProvider
{
  public:
    TileProvider() = default;

    // get the URL for a tile
    static std::string url(const char *filename);
    static std::string url(int z, int x, int y);

    // get all URL templates
    static const std::vector<std::string> &templates();

    // add a custom URL template
    static void addTemplate(const std::string &tmpl);

    // select/activate a template by index
    static void selectTemplate(int idx);

    // get the selected template index
    static int selectedTemplate(void);

  private:
    static std::vector<std::string> urlTemplates;
};
