#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include "../../lib/include/wwsfmllib.hpp"

namespace {
void PrintHelp() {
  std::cerr << "Supported arguments: \n"
               "--random-walker : Random/silly walk.\n"
               "--frame-buffer  : Pixel based frame buffer test.\n"
               "--frame-rate    : Show the frame rate.\n"
            << std::endl;
}
}; // namespace

/**
 */
int main(int argc, char const* argv[]) {

  /**
   * Create a map of the arguments to be supported.
   */
  std::map<std::string, bool> ArgRequested{};
  for (int Idx = 0; Idx < argc; ++Idx) {

    std::string const Arg(argv[Idx]);

    if ("--random-walker" == Arg) {
      ArgRequested["random-walker"] = true;
    }

    if ("--frame-buffer" == Arg) {
      ArgRequested["frame-buffer"] = true;
    }

    if ("--frame-rate" == Arg) {
      ArgRequested["frame-rate"] = true;
    }

    if ("--help" == Arg) {
      PrintHelp();
      return 0;
    }
  }

  /**
   * Hardcode a font to be used.
   */
  auto PathToFont = std::filesystem::path(argv[argc - argc]).remove_filename();
  PathToFont /= "../fonts/miracode/Miracode.ttf";

  sf::Vector2 WindowSize{1920.f, 1080.f};

  std::vector<std::shared_ptr<wwsfmlbase::ww_sfml_base>> vSfmlBaseObjects{};

  /**
   * Create the frame buffer that fills the entire canvas.
   */
  if (ArgRequested["frame-buffer"]) {
    auto pFrameBuffer = wwsfmlbase::CreateFrameBuffer(WindowSize.x, WindowSize.y);
    if (pFrameBuffer)
      vSfmlBaseObjects.push_back(pFrameBuffer);
  }

  /**
   * Create the Random Walker widget.
   */
  if (ArgRequested["random-walker"]) {
    auto pRandomWalker = wwsfmlbase::CreateRandomWalker();
    if (pRandomWalker)
      vSfmlBaseObjects.push_back(pRandomWalker);
  }

  /**
   * Create a Widget for the frame rate.
   */
  if (ArgRequested["frame-rate"]) {
    auto pFrameRate = wwsfmlbase::CreateFrameRate(200.f, 100.f, PathToFont);
    if (pFrameRate)
      vSfmlBaseObjects.push_back(pFrameRate);
  }

  auto const WW     = static_cast<unsigned int>(WindowSize.x);
  auto const WH     = static_cast<unsigned int>(WindowSize.y);
  auto       window = sf::RenderWindow{{WW, WH}, "CMake SFML Project"};

  window.setFramerateLimit(144);
  window.setVerticalSyncEnabled(true); // call it once, after creating the window

  while (window.isOpen()) {

    bool      PrintObj{};
    sf::Event EventToProcess{};

    for (auto event = sf::Event{}; window.pollEvent(event);) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::Resized) {
        WindowSize.x = static_cast<float>(event.size.width);
        WindowSize.y = static_cast<float>(event.size.height);
        std::cout << "new width: " << event.size.width << std::endl;
        std::cout << "new height: " << event.size.height << std::endl;
      }

      if (event.type == sf::Event::KeyPressed) {
        EventToProcess = event;
        if (event.key.scancode == sf::Keyboard::Scan::Escape) {
          std::cout << "the escape key was pressed" << std::endl;
          std::cout << "scancode: " << event.key.scancode << std::endl;
          std::cout << "code: " << event.key.code << std::endl;
          std::cout << "control: " << event.key.control << std::endl;
          std::cout << "alt: " << event.key.alt << std::endl;
          std::cout << "shift: " << event.key.shift << std::endl;
          std::cout << "system: " << event.key.system << std::endl;
          std::cout << "description: " << sf::Keyboard::getDescription(event.key.scancode).toAnsiString() << std::endl;
          std::cout << "localize: " << sf::Keyboard::localize(event.key.scancode) << std::endl;
          std::cout << "delocalize: " << sf::Keyboard::delocalize(event.key.code) << std::endl;
          PrintObj = true;
        }
      }
    }

    window.clear();

    /**
     * Update and draw all objects of interest.
     */
    for (auto& Obj : vSfmlBaseObjects) {

      Obj->Update(EventToProcess);
      window.draw(*Obj);

      if (PrintObj)
        Obj->Print();
    }

    window.display();
  }
}
