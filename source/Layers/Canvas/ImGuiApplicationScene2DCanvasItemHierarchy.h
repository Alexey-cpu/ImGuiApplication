
// STL
#include <list>

namespace ImGuiApplication
{
    namespace Scene2D
    {
        class Hierarchy
        {
        public:

            Hierarchy(Hierarchy* _Parent = nullptr)
            {
                setParent(_Parent);
            }

            virtual ~Hierarchy()
            {
                // remove all children
                auto children = m_Children;

                for(auto child : children)
                {
                    if(child != nullptr)
                        delete child;
                }

                // clear children list
                m_Children.clear();
            }

            void setParent(Hierarchy* _Parent)
            {
                // detach self from previous parent
                if(m_Parent != nullptr)
                    m_Parent->detachChild(this);

                // attach self to a new parent
                m_Parent = _Parent;
                if(m_Parent != nullptr)
                    m_Parent->attachChild(this);
            }

            template<typename __type> __type* getParent()
            {
                return dynamic_cast<__type*>(m_Parent);
            }

        protected:

            // hierarchy
            Hierarchy* m_Parent   = nullptr;

            std::list<Hierarchy*> m_Children =
                std::list<Hierarchy*>();

            // service methods
            void detachChild(Hierarchy* _Child)
            {
                auto iterator = std::find(m_Children.begin(), m_Children.end(), _Child);

                if(iterator != m_Children.end())
                    m_Children.erase(iterator);
            }

            void attachChild(Hierarchy* _Child)
            {
                if(_Child != nullptr)
                    m_Children.push_back(_Child);
            }
        };
    }
}
