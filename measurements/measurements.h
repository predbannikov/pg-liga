#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <cmath>
#include <limits>
#include <type_traits>
#include <QStringList>
#include <QMetaType>
#include <QtGlobal>
#include <QDebug>

#define _USE_MATH_DEFINES
#include <math.h>



const double DIVISION_EPS = 1.0e-8;

namespace Measurements {

template <class T> class Quantity
{
public:
    Quantity() = default;

    double siValue() const { return m_siValue; }
    void setSiValue(double value) { m_siValue = value; }
//  double relativeTo(Quantity other) const { return siValue() / other.siValue(); }
    double relativeTo(Quantity other) const
    {
        double divider = other.siValue();
        if ((divider < -DIVISION_EPS) || (divider > DIVISION_EPS)) return siValue() / divider;
        else   {/*qDebug () << "Measurements: divide by zero #1";*/ return 0.0;} /// @todo Возвращать код ошибки
    }
    bool isValid() const { return !std::isnan(m_siValue); }
    bool isZero() const { return m_siValue == 0; }

    T operator +(const T &other) const { return T(m_siValue + other.m_siValue); }
    T operator -(const T &other) const { return T(m_siValue - other.m_siValue); }
    T operator -() const { return  T(-m_siValue); }
    T operator *(double factor) const { return T(m_siValue * factor); }
//  T operator /(double factor) const { return T(m_siValue / factor); }
    T operator /(double factor) const
    {
        double divider = factor;
        if ((divider < -DIVISION_EPS) || (divider > DIVISION_EPS)) return T(m_siValue / divider);
        else   {/*qDebug () << "Measurements: divide by zero #2";*/ return 0.0;} /// @todo Возвращать код ошибки
    }

    void operator +=(const T &other) { m_siValue += other.m_siValue; }
    void operator -=(const T &other) { m_siValue -= other.m_siValue; }
    void operator *=(double factor) { m_siValue *= factor; }

//  double operator /(const T &other) const { return m_siValue / other.m_siValue; }
    double operator /(const T &other) const
    {
        double divider = other.m_siValue;
        if ((divider < -DIVISION_EPS) || (divider > DIVISION_EPS)) return m_siValue / divider;
        else   {/*qDebug () << "Measurements: divide by zero #3";*/ return 0.0;} /// @todo Возвращать код ошибки
    }

    bool operator ==(const T &other) const { return m_siValue == other.m_siValue; }
    bool operator !=(const T &other) const { return m_siValue != other.m_siValue; }
    bool operator >(const T &other) const { return m_siValue > other.m_siValue; }
    bool operator >=(const T &other) const { return m_siValue >= other.m_siValue; }
    bool operator <(const T &other) const { return m_siValue < other.m_siValue; }
    bool operator <=(const T &other) const { return m_siValue <= other.m_siValue; }

    static T invalid() { return T(std::numeric_limits<double>::quiet_NaN()); }

protected:
    Quantity(double value):
        m_siValue(value)
    {
        static_assert(std::is_base_of<Quantity,T>::value, "Has to be a subclass of Quantity");
    }

private:
    double m_siValue = 0;
};

class Length : public Quantity<Length>
{
    friend class Quantity<Length>;

public:
    Length() = default;

    static Length fromMetres(double m) { return Length(m); }
    static Length fromCentimetres(double cm) { return Length(cm / 1e2); }
    static Length fromMillimetres(double mm) { return Length(mm / 1e3); }
    static Length fromMicrometres(double um) { return Length(um / 1e6); }

    double metres() const { return siValue(); }
    double centimetres() const { return siValue() * 1e2; }
    double millimetres() const { return siValue() * 1e3; }
    double micrometres() const { return siValue() * 1e6; }

private:
    Length(double m): Quantity(m) {}
};

class TimeInterval : public Quantity<TimeInterval> {
    friend class Quantity<TimeInterval>;

public:
    TimeInterval() = default;

    static TimeInterval fromHMS(double h, double m, double s) { return TimeInterval(h * 3600.0 + m * 60.0 + s); }
    static TimeInterval fromStringShort(const QString &str) {
        QStringList splitted = str.split(' ');

        if(splitted.count() < 6) {
            return TimeInterval();

        } else {
            int hours = splitted.at(0).toInt();
            int minutes = splitted.at(2).toInt();
            int seconds = splitted.at(4).toInt();

            return TimeInterval::fromHMS(hours, minutes, seconds);
        }
    }

    QString toString() const { return QString("%1 ч %2 м %3 с").arg(QString::number(hour()),   2, '0')
                                        .arg(QString::number(minute()), 2, '0')
                                        .arg(QString::number(second()), 2, '0');
    }

    double milliseconds() const { return siValue() * 1000.0; }
    double seconds() const { return siValue(); }
    double minutes() const { return siValue() / 60.0; }
    double hours() const { return siValue() / 3600.0; }
    double days() const { return siValue() / 86400.0; }

    int second() const { return static_cast<int>(seconds()) % 60; }
    int minute() const { return static_cast<int>(minutes()) % 60; }
    int hour() const { return static_cast<int>(hours()) % 24; }
    int day() const { return static_cast<int>(days()) % 7; }

private:
    TimeInterval(double s): Quantity(s) {}
};

class TimeLongInterval : public Quantity<TimeLongInterval> {
    friend class Quantity<TimeLongInterval>;

public:
    TimeLongInterval() = default;

    static TimeLongInterval fromHMS(double h, double m, double s) { return TimeLongInterval(h * 3600.0 + m * 60.0 + s); }
    static TimeLongInterval fromDHM(double d, double h, double m) { return TimeLongInterval(d * 86400.0 + h * 3600.0 + m * 60.0); }
    static TimeLongInterval fromDHMS(double d, double h, double m, double s) { return TimeLongInterval(d * 86400.0 + h * 3600.0 + m * 60.0 + s); }
    static TimeLongInterval fromMinuts(double minuts) {
        int days = minuts / 1440;
        minuts = int(minuts) % 1440;
        int hours = minuts / 60;
        minuts = int(minuts) % 60;
        return fromDHM(days, hours, minuts);
    }
    static TimeLongInterval fromStringLong(const QString &str) {
        QStringList splitted = str.split(' ');

        if(splitted.count() < 6) {
            return TimeLongInterval();

        } else if (splitted.count() < 8) {
            int days = 0;
            int hours = splitted.at(0).toInt();
            int minutes = splitted.at(2).toInt();
            int seconds = splitted.at(4).toInt();
            return TimeLongInterval::fromDHMS(days, hours, minutes, seconds);
        } else {
            int days = splitted.at(0).toInt();
            int hours = splitted.at(2).toInt();
            int minutes = splitted.at(4).toInt();
            int seconds = splitted.at(6).toInt();
            return TimeLongInterval::fromDHMS(days, hours, minutes, seconds);
        }
    }

    QString toString() const { return QString("%1 д %2 ч %3 м %4 c").arg(QString::number(day()),   2, '0')
                                        .arg(QString::number(hour()), 2, '0')
                                        .arg(QString::number(minute()), 2, '0')
                                        .arg(QString::number(second()), 2, '0');
    }

    double milliseconds() const { return siValue() * 1000.0; }
    /**
     * @brief seconds   Возвращает время в секундах, чтобы получить часть содержащую секунды вызвать second()
     * @return
     */
    double seconds() const { return siValue(); }
    double minutes() const { return siValue() / 60.0; }
    double hours() const { return siValue() / 3600.0; }
    double days() const { return siValue() / 86400.0; }
    double months() const { return siValue() / 2678400.0; }  // 31 day

    /**
     * @brief second    Возвращает только часть секунд, чтобы получить время в секундах вызвать seconds()
     * @return
     */
    int second() const { return static_cast<int>(seconds()) % 60; }
    int minute() const { return static_cast<int>(minutes()) % 60; }
    int hour() const { return static_cast<int>(hours()) % 24; }
    int day() const { return static_cast<int>(days()) % 104; }   //override 7days limit
    int month() const { return static_cast<int>(months()) % 12; }

private:
    TimeLongInterval(double s): Quantity(s) {}
};

class Speed : public Quantity<Speed>
{
    friend class Quantity<Speed>;

public:
    Speed() = default;

    static Speed fromMillimetresPerMinute(double mmMin) { return Speed(mmMin * 1e-3 / 60.0); }

    double millimetresPerMinute() const { return siValue() * 1e3 * 60.0; }
    double millimetresPerSecond() const { return siValue() * 1e3; }
    double micrometresPerSecond() const { return siValue() * 1e6; }

private:
    Speed(double metresPerSecond): Quantity(metresPerSecond) {}
};

class Area : public Quantity<Area> {
    friend class Quantity<Area>;

public:
    Area() = default;

    static Area rectangle(Length length, Length width) { return Area(length.siValue() * width.siValue()); }
    static Area square(Length width) { return Area::rectangle(width, width); }
    static Area circle(Length diameter) { return Area(M_PI * pow(diameter.siValue() / 2.0, 2.0)); }

    static Area fromMetresSquare (double m2)        { return Area(m2); }
    static Area fromCentimetresSquared (double cm2) { return Area(cm2 / 1e4); }
    static Area fromMillimetresSquared (double mm2) { return Area(mm2 / 1e6); }

    double metresSquared() const { return siValue(); }
    double centimetresSquared() const { return siValue() * 1e4; }
    double millimetresSquared() const { return siValue() * 1e6; }

    Length length() const { return Length::fromMetres(2.0 * sqrt(siValue() / M_PI)); }

private:
    Area(double metresSquared): Quantity(metresSquared) {}
};

class Volume : public Quantity<Volume> {
    friend class Quantity<Volume>;

public:
    Volume() = default;

    static Volume box(Length length, Length width, Length height) { return Volume(length.siValue() * width.siValue() * height.siValue()); }
    static Volume cube(Length width) { return Volume::box(width, width, width); }
    static Volume cylinder(Length diameter, Length height) { return Volume(Area::circle(diameter).siValue() * height.siValue()); }

    static Volume fromCentimetresCubic(double cm3) { return Volume(cm3 * 1e-6); }
    static Volume fromMillimetresCubic(double mm3) { return Volume(mm3 * 1e-9); }

    double metresCubic() const { return siValue(); }
    double centimetresCubic() const { return siValue() * 1e6; }
    double millimetresCubic() const { return siValue() * 1e9; }

private:
    Volume(double metresCubic): Quantity(metresCubic) {}
};

class Force : public Quantity<Force>
{
    friend class Quantity<Force>;

public:
    Force() = default;

    static Force fromNewtons(double newtons) { return Force(newtons); }
    static Force fromKiloNewtons(double kN) { return Force(kN * 1e3); }
    static Force fromMegaNewtons(double megN) { return Force(megN * 1e6); }

    double newtons() const { return siValue(); }
    double kiloNewtons() const { return siValue() / 1e3; }
    double megaNewtons() const { return siValue() / 1e6; }

private:
    Force(double n): Quantity(n) {}
};

class Pressure : public Quantity<Pressure>
{
    friend class Quantity<Pressure>;

public:
    Pressure() = default;

//  static Pressure fromForce(Force force, Area area) { return Pressure(force.siValue() / area.siValue()); }
    static Pressure fromForce(Force force, Area area)
    {
        double divider = area.siValue();
        if ((divider < -DIVISION_EPS) || (divider > DIVISION_EPS)) return Pressure(force.siValue() / divider);
        else   {/*qDebug () << "Measurements: divide by zero #4";*/ return 0.0;} /// @todo Возвращать код ошибки
    }
    static Pressure fromPascals(double pa) { return Pressure(pa); }
    static Pressure fromKiloPascals(double kPa) { return Pressure(kPa * 1e3); }
    static Pressure fromMegaPascals(double megPa) { return Pressure(megPa * 1e6); }

    double pascals() const { return siValue(); }
    double kiloPascals() const { return siValue() / 1e3; }

    double megaPascals() const { return siValue() / 1e6; }
    QString kiloPascalsStr() const { return QString::number(siValue() / 1e3) + "_kPa"; }
    Force force(Area area) const { return Force::fromNewtons(siValue() * area.siValue()); }

private:
    Pressure(double pa): Quantity(pa) {}
};

class Density : public Quantity<Density> {

    friend class Quantity<Density>;

public:
    Density() = default;

    static Density fromKilogrammPerCubicMeter(double kgPerM3) { return Density(kgPerM3); }
    static Density fromGrammPerCubicCentimeter(double gPerCm3) { return Density(gPerCm3 * 1e3); }

    double kilogrammPerCubicMeter() const { return siValue(); }
    double grammPerCubicCentimeter() const { return siValue() / 1e3; }

private:
    Density(double kgPerM3): Quantity(kgPerM3) {}
};

}

Q_DECLARE_METATYPE(Measurements::Pressure)
Q_DECLARE_METATYPE(Measurements::TimeInterval)
Q_DECLARE_METATYPE(Measurements::TimeLongInterval)
Q_DECLARE_METATYPE(Measurements::Volume)

#endif // MEASUREMENTS_H
