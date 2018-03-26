/*
  Permission for the use of this code is granted only for research, educational, and non-commercial purposes.

  Redistribution of this code or its parts in source, binary, and any other form without permission, with or without modification, is prohibited.  Modifications include, but are not limited to, translation to other programming languages and reuse of tables, constant definitions, and API's defined in it.

  Andrew Choi is not liable for any losses or damages caused by the use of this code.

  Copyright 2009 Andrew Choi.
*/

#ifndef QUAPPLICATION_H
#define QUAPPLICATION_H

#include "QUSingleApplication.h"

class QEvent;

class QUApplication : public QUSingleApplication
{
public:
    QUApplication(int &argc, char **argv);
    ~QUApplication();

#ifdef Q_OS_MAC
    bool event(QEvent *);
#endif
};

#endif // QTDOCBASEDAPPLICATION_H
