#include "QXResource.h"

namespace QXResource {
    QString
    path(const QString & name) {
        return FX_RESOURCES_DIR  + name;
    }
}
