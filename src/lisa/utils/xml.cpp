//
// Created by kinami on 4/17/26.
//

#include "xml.h"

#include "logging.h"
#include "path.h"

namespace lisa::utils::xml {

  pugi::xml_document read(const path& filepath, const str& top_level) {
    pugi::xml_document doc;

    if (const auto result = doc.load_file(pstr(filepath).c_str())) {
      logging::debug("XML file '{}' parsed correctly", pstr(filepath));
    } else {
      logging::abort(
        "Error parsing XML file\n\tDescription: {}\n\tAt: {}",
        result.description(),
        pstr(filepath).c_str() + result.offset
      );
    }

    if (!top_level.compare("") &&
        top_level.compare(doc.document_element().name()) != 0) {
      logging::abort(
        "XML definition must contain exactly one <{}></{}> node in the "
        "top-level",
        top_level,
        top_level
      );
    }

    return doc;
  }
}
