#include <SFML/Graphics/RenderWindow.hpp>

#include <Graphics/DrawUtils.hpp>
#include <Transform/Rect.hpp>
#include <Types/Any.hpp>
#include <Utils/MathUtils.hpp>

namespace obe
{
    namespace Transform
    {
        void Rect::transformRef(UnitVector& vec, Referencial ref, ConversionType type) const
        {
            double factor = (type == ConversionType::From) ? 1.0 : -1.0;
            switch (ref)
            {
            case Referencial::TopLeft:
                break;
            case Referencial::Top:
                vec.add(factor * m_size.to(vec.unit).x / 2, 0);
                break;
            case Referencial::TopRight:
                vec.add(factor * m_size.to(vec.unit).x, 0);
                break;
            case Referencial::Left:
                vec.add(0, factor * m_size.to(vec.unit).y / 2);
                break;
            case Referencial::Center:
                vec.add(factor * m_size.to(vec.unit).x / 2, factor * m_size.to(vec.unit).y / 2);
                break;
            case Referencial::Right:
                vec.add(factor * m_size.to(vec.unit).x, factor * m_size.to(vec.unit).y / 2);
                break;
            case Referencial::BottomLeft:
                vec.add(0, factor * m_size.to(vec.unit).y);
                break;
            case Referencial::Bottom:
                vec.add(factor * m_size.to(vec.unit).x / 2, factor * m_size.to(vec.unit).y);
                break;
            case Referencial::BottomRight:
                vec.add(factor * m_size.to(vec.unit).x, factor * m_size.to(vec.unit).y);
                break;
            default: break;
            }
        }

        void Rect::setPosition(const UnitVector& position, Referencial ref)
        {
            UnitVector pVec = position.to<Units::WorldUnits>();
            this->transformRef(pVec, ref, ConversionType::To);
            m_position.set(pVec.x, pVec.y);
        }

        void Rect::setPosition(double x, double y, Referencial ref)
        {
            this->setPosition(UnitVector(x, y, Units::WorldUnits), ref);
        }

        void Rect::move(const UnitVector& position)
        {
            m_position += position;
        }

        void Rect::move(double x, double y)
        {
            m_position.add(x, y);
        }

        void Rect::setX(double x, Referencial ref)
        {
            UnitVector vec(x, 0, Units::WorldUnits);
            this->transformRef(vec, ref, ConversionType::To);
            m_position.x = vec.x;
        }

        void Rect::setY(double y, Referencial ref)
        {
            UnitVector vec(0, y, Units::WorldUnits);
            this->transformRef(vec, ref, ConversionType::To);
            m_position.y = vec.y;
        }

        void Rect::setSize(double width, double height, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size.set(width, height);
            this->setPosition(savePosition, ref);
        }

        void Rect::setSize(const UnitVector& size, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size.set(size);
            this->setPosition(savePosition, ref);
        }

        void Rect::scale(const UnitVector& size, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size *= size;
            this->setPosition(savePosition, ref);
        }

        void Rect::scale(double width, double height, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size *= UnitVector(width, height, m_size.unit);
            this->setPosition(savePosition, ref);
        }

        void Rect::setWidth(double width, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size.x = width;
            this->setPosition(savePosition, ref);
        }

        void Rect::setHeight(double height, Referencial ref)
        {
            UnitVector savePosition = this->getPosition(ref);
            m_size.y = height;
            this->setPosition(savePosition, ref);
        }

        UnitVector Rect::getPosition(Referencial ref) const
        {
            UnitVector getPosVec = m_position;
            this->transformRef(getPosVec, ref, ConversionType::From);
            return getPosVec;
        }

        UnitVector Rect::getSize() const
        {
            return m_size;
        }

        double Rect::getX(Referencial ref) const
        {
            return this->getPosition(ref).x;
        }

        double Rect::getY(Referencial ref) const
        {
            return this->getPosition(ref).y;
        }

        double Rect::getWidth() const
        {
            return m_size.x;
        }

        double Rect::getHeight() const
        {
            return m_size.y;
        }

        void Rect::setPointPosition(const UnitVector& position, Referencial ref)
        {
            UnitVector oppositePointPosition = this->getPosition(reverseReferencial(ref));
            UnitVector newSize = (oppositePointPosition - position) * getReferencialOffset(ref);
            if (isOnCorner(ref))
            {
                this->setPosition(position, ref);
                this->setSize(newSize, ref);
            }
            else if (isOnLeftSide(ref) || isOnRightSide(ref))
            {
                this->setX(position.to<Units::WorldUnits>().x, ref);
                this->setWidth(newSize.x, ref);
            }
            else if (isOnTopSide(ref) || isOnBottomSide(ref))
            {
                this->setY(position.to<Units::WorldUnits>().y, ref);
                this->setHeight(newSize.y, ref);
            }
        }

        void Rect::setPointPosition(double x, double y, Referencial ref)
        {
            UnitVector pVec(x, y);
            this->setPointPosition(pVec, ref);
        }

        void Rect::movePoint(const UnitVector& position, Referencial ref)
        {
        }

        void Rect::movePoint(double x, double y, Referencial ref)
        {
        }

        void Rect::setPointX(double x, Referencial ref)
        {
        }

        void Rect::setPointY(double y, Referencial ref)
        {
        }

        UnitVector Rect::getScaleFactor() const
        {
            return UnitVector(Utils::Math::sign(m_size.x), Utils::Math::sign(m_size.y));
        }

        void Rect::display(sf::RenderWindow& target) const
        {
            int r = 6;
            std::map<std::string, Types::Any> drawOptions;

            drawOptions["lines"] = true;
            drawOptions["points"] = true;
            drawOptions["radius"] = r;
            drawOptions["point_color"] = sf::Color::White;

            std::vector<sf::Vector2i> drawPoints;

            drawOptions["point_color_0"] = sf::Color(255, 0, 0);
            drawOptions["point_color_1"] = sf::Color(255, 128, 0);
            drawOptions["point_color_2"] = sf::Color(255, 255, 0);
            drawOptions["point_color_3"] = sf::Color(128, 255, 0);
            drawOptions["point_color_4"] = sf::Color(0, 255, 0);
            drawOptions["point_color_5"] = sf::Color(0, 255, 128);
            drawOptions["point_color_6"] = sf::Color(0, 255, 255);
            drawOptions["point_color_7"] = sf::Color(0, 128, 255);
            drawOptions["point_color_8"] = sf::Color(0, 0, 255);

            UnitVector pixelPosition = m_position.to<Units::WorldPixels>();
            UnitVector pixelSize = m_size.to<Units::WorldPixels>();

            drawPoints.emplace_back(pixelPosition.x + r, pixelPosition.y + r);
            drawPoints.emplace_back(pixelPosition.x + pixelSize.x / 2, pixelPosition.y + r);
            drawPoints.emplace_back(pixelPosition.x + pixelSize.x - r, pixelPosition.y + r);
            drawPoints.emplace_back(pixelPosition.x + pixelSize.x - r, pixelPosition.y + pixelSize.y / 2);
            drawPoints.emplace_back(pixelPosition.x + pixelSize.x - r, pixelPosition.y + pixelSize.y - r);
            drawPoints.emplace_back(pixelPosition.x + pixelSize.x / 2, pixelPosition.y + pixelSize.y - r);
            drawPoints.emplace_back(pixelPosition.x + r, pixelPosition.y + pixelSize.y - r);
            drawPoints.emplace_back(pixelPosition.x + r, pixelPosition.y + pixelSize.y / 2);

            Graphics::Utils::drawPolygon(target, drawPoints, drawOptions);
        }

        bool Rect::intersects(const Rect& rect) const
        {
            UnitVector p1 = m_position.to<Units::WorldUnits>();
            UnitVector p2 = rect.getPosition().to<Units::WorldUnits>();
            UnitVector s1 = m_size.to<Units::WorldUnits>();
            UnitVector s2 = rect.getSize().to<Units::WorldUnits>();
            return (abs(p1.x - p2.x) < (s1.x + s2.x) / 2) &&
                (abs(p1.y - p2.y) < (s1.y + s2.y) / 2);
        }
    }
}
