#include <fstream>
#include <iostream>

#include <vili/Vili.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <Bindings/Bindings.hpp>
#include <Editor/MapEditor.hpp>
#include <Graphics/LevelSprite.hpp>
#include <Modes/Game.hpp>
#include <Modes/Menu.hpp>
#include <Modes/Toolkit.hpp>
#include <ObEngine.hpp>
#include <System/MountablePath.hpp>
#include <Transform/UnitVector.hpp>
#include <Types/Any.hpp>
#include <Utils/ExecUtils.hpp>
#include <Utils/TypeUtils.hpp>

void LoadErrors()
{
    vili::LoadErrors("Data/Errors.vili");
}

using namespace obe;

class Injectore
{
    private:
        kaguya::detail::LuaVariantImpl<kaguya::TableKeyReferenceProxy<char const *>> injectValue;
    public:
        Injectore();
        void inject(kaguya::detail::LuaVariantImpl<kaguya::TableKeyReferenceProxy<char const *>> value);
        void get(kaguya::State* lua);
        
};

int main(int argc, char** argv)
{
    Utils::Exec::RunArgsParser runParser(argc, argv);
    std::string startMode = runParser.getArgumentValue("-mode");
    std::cout << "Running ObEngine using mode : " << startMode << std::endl;

    Transform::UnitVector::Init(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height);
    Graphics::PositionTransformers::Init();

    /*std::ofstream out("debug.log");
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());*/

    /*std::ofstream out_err("errors.log");
    std::streambuf* cerrbug = std::cerr.rdbuf();
    std::cerr.rdbuf(out_err.rdbuf());*/

    std::cout << "<Computer Configuration>" << std::endl;
    std::cout << "Screen Resolution : " << Transform::UnitVector::Screen.w << ", " << Transform::UnitVector::Screen.h << std::endl;

    LoadErrors();
    System::MountPaths();

    Bindings::IndexBindings();

    if (startMode == "edit")
    {
        std::string editMapName = Modes::chooseMapMenu();
        if (editMapName != "")
            Editor::editMap(editMapName);
    }
    else if (startMode == "play")
        Modes::startGame();
    else if (startMode == "toolkit")
        Modes::startToolkitMode();
    else if (startMode == "dev")
        Modes::startDevMenu();
    else
        Modes::startGame();

    return 0;
}
