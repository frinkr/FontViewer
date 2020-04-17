#pragma once
#include <iterator>
#include "FXIterator.h"

using FXFileIterator = FXIterator<FXString>;

FXPtr<FXFileIterator> FXCreateSystemFontFileIterator();
