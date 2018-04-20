#include "QUResource.h"

namespace QUResource {
    QString
    path(const QString & name) {
        return FX_RESOURCES_DIR  + name;
    }
}
