#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

const int W = 1920;
const int H = 1080;
double zoom = 1.0;


int max_iter = 128;
double min_re = -2.5, max_re = 1;
double min_im = -1, max_im = 1;

Color linear_interpolation(const Color& v, const Color& u, double a)
{
	auto const b = 1 - a;
	return Color(b*v.r + a * u.r, b*v.g + a * u.g, b*v.b + a * u.b);
}


int main(void)
{
    RenderWindow window(VideoMode(W, H), "Mandelbrot Fractal");
    Image image; 
    image.create(W, H);
    Texture texture;
    Sprite sprite;

    Font font; font.loadFromFile("arial.ttf");
	Text text;
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(Color::White);

    

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseWheelScrolled){
                if (event.MouseWheelScrolled){
                    if(event.mouseWheelScroll.wheel == Mouse::VerticalWheel){
                        if(event.mouseWheelScroll.delta > 0) max_iter *= 2;
                        else max_iter /= 2;
                        if(max_iter < 1) max_iter = 1;
                    }
                }
            }
            if (event.type == Event::KeyPressed){
                double w = (max_re - min_re)*0.3;
                double h = (max_im - min_im)*0.3;

                if (event.key.code == Keyboard::Left){min_re-= w, max_re -= w;}
                if (event.key.code == Keyboard::Right){min_re += w, max_re += w;}
                if(event.key.code == Keyboard::Up){min_im -= h, max_im -= h;}
                if(event.key.code == Keyboard::Down){min_im += h, max_im += h;}
            }
            if (event.type == Event::MouseButtonPressed){
                auto zoom_x = [&](double z)
                    {
                    double cr = min_re + (max_re-min_re) * event.mouseButton.x / W;
                    double ci = min_im + (max_im - min_im) * event.mouseButton.y / H;

                    double tminr = cr - (max_re - min_re)/2/z;
                    max_re = cr + (max_re - min_re) / 2 / z;
                    min_re = tminr;

                    double tmini = ci - (max_im - min_im) / 2 / z;
                    max_im = ci + (max_im - min_im) / 2 / z;
                    min_im = tmini;
                };
                if (event.mouseButton.button == Mouse::Left)
				{
					zoom_x(5);
					zoom *= 5;
				}
				//Right Click to ZoomOut
				if (event.mouseButton.button == Mouse::Right)
				{
					zoom_x(1.0 / 5);
					zoom /= 5;
				}
            }
                
        }

        window.clear();

        for (int y = 0; y < H; y++) for (int x = 0; x < W; x++){
            
            double cr = min_re + (max_re-min_re) * x / W;
            double ci = min_im + (max_im - min_im) * y / H;
            double re = 0, im = 0;
            int iter;
            for (iter = 0; iter < max_iter; iter++){
                double tr = re * re - im * im + cr;
                im = 2 * re * im + ci;
                re = tr;
                if ( re * re + im * im > 2 * 2) break;
            } 
            int r = 1.0*(max_iter - iter) / max_iter * 0xff;
            int g = r, b = r;

            static const vector<Color> colors{
				{0,0,0},
				{213,67,31},
				{251,255,121},
				{62,223,89},
				{43,30,218},
				{0,255,247}
			};

            static const vector<Color> wiki_color_palette{
                {0, 7, 0},
                {32, 107, 203},
                {237, 255, 255},
                {255, 170, 0},
                {0, 2, 0}
            };

			static const auto max_color = wiki_color_palette.size() - 1;
			if (iter == max_iter)iter = 0;
			double mu = 1.0*iter / max_iter;
			//scale mu to be in the range of colors
			mu *= max_color;
			auto i_mu = static_cast<size_t>(mu);
			auto color1 = wiki_color_palette[i_mu];
			auto color2 = wiki_color_palette[min(i_mu + 1, max_color)];
			Color c = linear_interpolation(color1, color2, mu - i_mu);

            image.setPixel(x, y, Color(c));
        }
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);

        char str[100];
		sprintf(str, "max iter:%d\nzoom:x%2.2lf", max_iter, zoom);
		text.setString(str);
		window.draw(text);


        window.display();
    }

    return 0;
}