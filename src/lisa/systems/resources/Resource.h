//
// Created by kinami on 3/29/26.
//

#ifndef LISA_RESOURCE_H
#define LISA_RESOURCE_H
#include "utils/common.h"
#include "utils/logging.h"

namespace lisa::systems::resources {

  class Resource {
  public:
    explicit Resource(const str& id) : id_(id) {}

    virtual ~Resource() = default;

    const str& id() const { return id_; }

    bool is_loaded() const { return loaded_; }

    bool load();

    void unload() {
      unload_function();
      loaded_ = false;
    }

  protected:
    str id_;
    bool loaded_ = false;

    virtual bool load_function() = 0;
    virtual bool unload_function() = 0;
    virtual str type_name() = 0;
  };

}

#endif // LISA_RESOURCE_H
