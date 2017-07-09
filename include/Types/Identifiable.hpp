#pragma once

#include <string>

namespace obe
{
    namespace Types
    {
        /**
         * \brief A Base class that does carry an unique identifier for a given category (Sprites / Colliders etc..)
         */
        class Identifiable
        {
        protected:
            std::string m_id;
        public:
            /**
            * \brief Creates a new Identifiable
            * \param id A std::string containing the id of the Identifiable
            */
            explicit Identifiable(const std::string& id);
            /**
             * \brief Set a new id for the Identifiable
             * \param id Sets the id of the Identifiable
             */
            void setId(const std::string& id);
            /**
             * \brief Get the id of the Identifiable
             * \return A std::string containing the id of the Identifiable
             */
            std::string getId() const;
        };

        /**
         * \brief Exactly like Identifiable class except you can't change the Id after creation
         */
        class ProtectedIdentifiable : public Identifiable
        {
        private:
            /**
            * \brief Set a new id for the Identifiable
            * \param id Sets the id of the Identifiable
            */
            void setId(const std::string& id);
        };
    }
}