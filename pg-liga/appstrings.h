#ifndef APPSTRINGS_H
#define APPSTRINGS_H

#include <QObject>

namespace Strings {

static const auto calibrate = QObject::tr("Тарировать");
static const auto zero = QObject::tr("Обнулить");
static const auto reset = QObject::tr("Сбросить ноль");

static const auto force = QObject::tr("усилие");
static const auto deform = QObject::tr("деформация");
static const auto pressure = QObject::tr("давление");
static const auto position = QObject::tr("положение");

static const auto kPa = QObject::tr("кПа");
static const auto kN = QObject::tr("кН");
static const auto N = QObject::tr("Н");
static const auto mm = QObject::tr("мм");
static const auto mm3 = QObject::tr("мм³");
static const auto cm3 = QObject::tr("см³");
static const auto percent = QObject::tr("%");

static const auto mmMin = QObject::tr("мм/мин");
static const auto kPaSec = QObject::tr("кПа/с");

static const auto sigma = QObject::tr("σ");
static const auto epsilon = QObject::tr("ε");

}

#endif // APPSTRINGS_H
