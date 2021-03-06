// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of thekogans_make_core.
//
// thekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// thekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with thekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_core_Generator_h)
#define __thekogans_make_core_Generator_h

#include <memory>
#include <string>
#include <list>
#include <map>
#include "pugixml/pugixml.hpp"
#include "thekogans/util/Types.h"
#include "thekogans/util/RefCounted.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            /// \struct Generator Generator.h thekogans/make/Generator.h
            ///
            /// \brief
            /// Base class used to represent an abstract build system generator.

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Generator : public util::RefCounted {
                /// \brief
                /// Convenient typedef for util::RefCounted::SharedPtr<Generator>.
                typedef util::RefCounted::SharedPtr<Generator> SharedPtr;

                /// \brief
                /// typedef for the Generator factory function.
                typedef SharedPtr (*Factory) (bool rootProject);
                /// \brief
                /// typedef for the Generator map.
                typedef std::map<std::string, Factory> Map;
                /// \brief
                /// Used for Generator dynamic discovery and creation.
                /// \param[in] type Generator type (it's name).
                /// \param[in] rootProject true == root project, false == child project.
                /// \return A Generator based on the passed in type.
                static SharedPtr Get (
                    const std::string &type,
                    bool rootProject);
                /// \struct MapInitializer Generator.h thekogans/make/Generator.h
                ///
                /// \brief
                /// MapInitializer is used to initialize the Generator::map.
                /// It should not be used directly, and instead is included
                /// in THEKOGANS_MAKE_CORE_DECLARE_GENERATOR/THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR.
                /// If you are deriving a generator from Generator, and you want
                /// it to be dynamically discoverable/creatable, add
                /// THEKOGANS_MAKE_CORE_DECLARE_GENERATOR to it's declaration,
                /// and THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR to it's definition.
                struct _LIB_THEKOGANS_MAKE_CORE_DECL MapInitializer {
                    /// \brief
                    /// ctor. Add generator of type, and factory for creating it
                    /// to the Generator::map
                    /// \param[in] type Generator type (it's class name).
                    /// \param[in] factory Generator creation factory.
                    MapInitializer (
                        const std::string &type,
                        Factory factory);
                };
                /// \brief
                /// Get the list of all generators registered with the map.
                /// \param[out] generators List of registered generetors.
                static void GetGenerators (std::list<std::string> &generators);

            protected:
                /// \brief
                /// true == root project, false == child project.
                bool rootProject;

            public:
                /// \brief
                /// ctor.
                /// \param[in] rootProject_ true == root project, false == child project.
                Generator (bool rootProject_) :
                    rootProject (rootProject_) {}

                /// \brief
                /// dtor.
                virtual ~Generator () {}

                /// \brief
                /// Return the class name of the generator.
                /// \return Class name of the generator.
                virtual const char *GetName () const = 0;

                /// \brief
                /// Generate a build system.
                /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
                /// \param[in] config Debug or Release.
                /// \param[in] type Static or Shared.
                /// \param[in] generateDependencies true = Generate Dependencies.
                /// \param[in] force true = Don't bother checking the timestamps and force generation.
                /// \return true = Generated the build system, false = The build system was up to date.
                virtual bool Generate (
                    const std::string &project_root,
                    const std::string &config,
                    const std::string &type,
                    bool generateDependencies,
                    bool force) = 0;

                /// \brief
                /// Delete a build system.
                /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
                /// \param[in] config Debug or Release.
                /// \param[in] type Static or Shared.
                /// \param[in] deleteDependencies true = Delete Dependencies.
                virtual void Delete (
                    const std::string &project_root,
                    const std::string &config,
                    const std::string &type,
                    bool deleteDependencies) = 0;
            };

            /// \brief
            /// Dynamic discovery macro. Add this to your class declaration.
            /// Example:
            /// \code{.cpp}
            /// struct vs2010 : public Generator {
            ///     THEKOGANS_MAKE_CORE_DECLARE_GENERATOR (vs2010)
            ///     ...
            /// };
            /// \endcode
            #define THEKOGANS_MAKE_CORE_DECLARE_GENERATOR(type)\
            public:\
                static thekogans::make::core::Generator::MapInitializer mapInitializer;\
                static thekogans::make::core::Generator::SharedPtr Create (bool rootProject) {\
                    return thekogans::make::core::Generator::SharedPtr (new type (rootProject));\
                }\
                virtual const char *GetName () const {\
                    return #type;\
                }

            /// \brief
            /// Dynamic discovery macro. Instantiate one of these in the class cpp file.
            /// Example:
            /// \code{.cpp}
            /// THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR (vs2010)
            /// \endcode
            #define THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR(type)\
                thekogans::make::core::Generator::MapInitializer type::mapInitializer (\
                    #type, type::Create);

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Generator_h)
