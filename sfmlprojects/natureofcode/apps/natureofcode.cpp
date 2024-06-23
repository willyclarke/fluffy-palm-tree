#include <filesystem>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

class frame_buffer : public sf::Drawable, public sf::Transformable {
public:
  // add functions to play with the entity's geometry / colors / texturing...

  void Init(size_t Win, size_t Hin) {

    m_vertices.resize(Win * Hin);
    W = Win;
    H = Hin;

    for (size_t Y = 0; //!<
         Y < Hin;      //!<
         ++Y)          //!<
    {
      for (size_t X = 0; //!<
           X < Win;      //!<
           ++X)          //!<
      {
        if ((X * Y) < m_vertices.getVertexCount())
          m_vertices[X + Y * W] = sf::Vertex(sf::Vector2f(float(X), float(Y)),
                                             sf::Color(static_cast<char>(Y % 256), static_cast<char>(X % 256), 128),
                                             sf::Vector2f(0.f, 0.f));
      }
    }
  }

  frame_buffer(size_t Win, size_t Hin) : W(Win), H(Hin) { Init(W, H); }
  frame_buffer() { Init(W, H); }
  ~frame_buffer() {}

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    // apply the texture
    states.texture = &m_texture;

    // you may also override states.shader or states.blendMode if you want

    // draw the vertex array
    target.draw(m_vertices, states);
  }

  size_t          W{1080};
  size_t          H{768};
  sf::VertexArray m_vertices{sf::Points, W* H};
  sf::Texture     m_texture;
};

class random_walker : public sf::Drawable, public sf::Transformable {
public:
  // add functions to play with the entity's geometry / colors / texturing...

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    // apply the texture
    states.texture = &m_texture;

    // you may also override states.shader or states.blendMode if you want

    // draw the vertex array
    target.draw(m_vertices, states);
  }

  sf::VertexArray m_vertices;
  sf::Texture     m_texture;
};

int main(int argc, char const* argv[]) {

  std::string const AppName(argv[0]);
  auto              ExePath = std::filesystem::current_path();
  sf::Font          Font;
  std::string const FontFile = ExePath.string() + "/fonts/miracode/Miracode.ttf";

  if (!Font.loadFromFile(FontFile)) {
    std::cerr << "Could not load font from file " << FontFile << std::endl;
    std::cerr << "Fonts are expected to be found in ../fonts as seen from the apps location." << std::endl;
  }

  constexpr int         NumPix{24};
  sf::Text              Text{};
  std::vector<sf::Text> vText{10};

  {
    int Idx{};
    for (auto& T : vText) {
      T.setFont(Font);
      T.setString("Hello world" + std::to_string(Idx));
      T.setCharacterSize(NumPix); // in pixels, not points!
      T.setFillColor(sf::Color::Blue);
      T.setStyle(sf::Text::Bold | sf::Text::Underlined);
      sf::Vector2 Position{0.f, static_cast<float>((1 + Idx) * (NumPix + 2))};
      T.setPosition(Position);
      ++Idx;
    }
  }

  Text.setFont(Font);
  Text.setString("Hello world");
  Text.setCharacterSize(NumPix); // in pixels, not points!
  Text.setFillColor(sf::Color::Green);
  Text.setStyle(sf::Text::Bold | sf::Text::Underlined);

  sf::Vector2 Position{0.f, 0.f};
  Text.setPosition(Position);
  Text.getPosition();

  sf::Vector2  WindowSize{1920.f, 1080.f};
  frame_buffer FrameBuffer{static_cast<size_t>(WindowSize.x), static_cast<size_t>(WindowSize.y)};

  auto const WW     = static_cast<unsigned int>(WindowSize.x);
  auto const WH     = static_cast<unsigned int>(WindowSize.y);
  auto       window = sf::RenderWindow{{WW, WH}, "CMake SFML Project"};

  window.setFramerateLimit(144);
  window.setVerticalSyncEnabled(true); // call it once, after creating the window

  sf::Clock Clock{};
  auto      TimePrv = Clock.getElapsedTime();

  float NumIter{};
  float FrameRateAvg{};
  while (window.isOpen()) {

    /**
     * Calculate frame rate.
     */
    {
      auto const TimeN1    = Clock.getElapsedTime();
      auto const DeltaTime = TimeN1 - TimePrv;
      TimePrv              = TimeN1;

      if (DeltaTime.asSeconds() > 0.f) {
        auto const FrameRate = 1.f / (DeltaTime.asSeconds());
        Text.setString(std::to_string(FrameRate));

        /* Compute running average */
        ++NumIter;
        auto const Avg = FrameRateAvg / NumIter;
        FrameRateAvg   = FrameRate + Avg;
        vText.at(0).setString(std::to_string(FrameRateAvg));
        window.draw(vText.at(0));
      }
    }

    bool PrintMatrix{};

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
          PrintMatrix = true;
        } else if (event.key.scancode == sf::Keyboard::Scan::A) {
          FrameBuffer.setRotation(FrameBuffer.getRotation() - 1.f);
        } else if (event.key.scancode == sf::Keyboard::Scan::B) {
          FrameBuffer.setRotation(FrameBuffer.getRotation() + 1.f);
        } else if (event.key.scancode == sf::Keyboard::Scan::Right) {
          Position.x += 1;
          FrameBuffer.setPosition(Position.x, Position.y);
        } else if (event.key.scancode == sf::Keyboard::Scan::Left) {
          Position.x -= 1;
          FrameBuffer.setPosition(Position.x, Position.y);
        } else if (event.key.scancode == sf::Keyboard::Scan::Up) {
          Position.y += 1;
          FrameBuffer.setPosition(Position.x, Position.y);
        } else if (event.key.scancode == sf::Keyboard::Scan::Down) {
          Position.y -= 1;
          FrameBuffer.setPosition(Position.x, Position.y);
        }
      }
    }

    if (PrintMatrix) {
      auto const pMatrix = FrameBuffer.getTransform().getMatrix();
      std::cout << "FrameBuffer.getTransform().getMatrix() gives\n" << std::endl;
      std::cout << " [ 0]:" << std::fixed << std::setprecision(3)                //!<
                << std::setfill(' ') << std::left << std::setw(10) << pMatrix[0] //!<
                << " [ 1]:" << std::setw(10) << pMatrix[1]                       //!<
                << " [ 2]:" << std::setw(10) << pMatrix[2]                       //!<
                << " [ 3]:" << std::setw(10) << pMatrix[3] << "\n"               //!<
                << " [ 4]:" << std::setw(10) << pMatrix[4]                       //!<
                << " [ 5]:" << std::setw(10) << pMatrix[5]                       //!<
                << " [ 6]:" << std::setw(10) << pMatrix[6]                       //!<
                << " [ 7]:" << std::setw(10) << pMatrix[7] << "\n"               //!<
                << " [ 8]:" << std::setw(10) << pMatrix[8]                       //!<
                << " [ 9]:" << std::setw(10) << pMatrix[9]                       //!<
                << " [10]:" << std::setw(10) << pMatrix[10]                      //!<
                << " [11]:" << std::setw(10) << pMatrix[11] << "\n"              //!<
                << " [12]:" << std::setw(10) << pMatrix[12]                      //!<
                << " [13]:" << std::setw(10) << pMatrix[13]                      //!<
                << " [14]:" << std::setw(10) << pMatrix[14]                      //!<
                << " [15]:" << std::setw(10) << pMatrix[15]                      //!<
                << "\n"
                << std::endl;
      ;
    }

    Text.setPosition(Position);

    if (Position.x > WindowSize.x)
      Position.x = 0;
    if (Position.y > WindowSize.y)
      Position.y = 0;

    window.clear();

    window.draw(FrameBuffer);
    window.draw(Text);

    for (size_t Idx = vText.size() - 1; Idx > 0; --Idx) {
      auto& T = vText.at(Idx);
      if (!(static_cast<long>(NumIter) % 60)) {
        auto const& Tp = vText.at(Idx - 1);
        T.setString(Tp.getString());
      }
      window.draw(T);
    }

    window.display();
  }
}
