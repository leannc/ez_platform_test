#pragma once

#include <string>

namespace CAD {

/*! \class Layer
 * \brief Define imGui layer basic class.
*/
class Layer
{
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnImGuiRender() {}

        /*! A method to get layer name. */
        inline const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };

}
