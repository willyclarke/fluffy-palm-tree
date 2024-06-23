#include <filesystem>
#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

int main(int argc, char const* argv[]) {

  std::string const AppName(argv[0]);
  auto              ExePath = std::filesystem::current_path();
  sf::Font          Font;
  std::string const FontFile = ExePath.string() + "/fonts/miracode/Miracode.ttf";

  if (!Font.loadFromFile(FontFile)) {
    std::cerr << "Could not load font from file " << FontFile << std::endl;
    std::cerr << "Fonts are expected to be found in ../fonts as seen from the apps location." << std::endl;
  }

  sf::Text Text;
  Text.setFont(Font);
  Text.setString("Hello world");
  Text.setCharacterSize(24); // in pixels, not points!
  Text.setFillColor(sf::Color::Red);
  Text.setStyle(sf::Text::Bold | sf::Text::Underlined);

  sf::Vector2 Position{20.f, 20.f};
  Text.setPosition(Position);

  sf::Vector2 WindowSize{1920.f, 1080.f};
  auto const  WW     = static_cast<unsigned int>(WindowSize.x);
  auto const  WH     = static_cast<unsigned int>(WindowSize.y);
  auto        window = sf::RenderWindow{{WW, WH}, "CMake SFML Project"};

  window.setFramerateLimit(144);
  window.setVerticalSyncEnabled(true); // call it once, after creating the window

  while (window.isOpen()) {
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
        }
      }
    }

    Position.x += 1;
    Position.y += 1;
    Text.setPosition(Position);

    if (Position.x > WindowSize.x)
      Position.x = 0;
    if (Position.y > WindowSize.y)
      Position.y = 0;

    window.clear();

    window.draw(Text);

    window.display();
  }
}
