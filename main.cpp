#include "bitmap.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <cstdio>


void line(Bitmap& bmp, unsigned x0, unsigned y0, unsigned x1, unsigned y1, unsigned color)
{

    float t = 0;
    float dy = int(y1 - y0);
    float dx = int(x1 - x0);

    t = dy / dx; 

    //std::cout << "x0: " << x0 << " y0 " << y0 << " x1 " << x1 << " y1 " << y1 << std::endl;
    //std::cout << "t: " << t << std::endl;
    if(abs(dx) > abs(dy))
    {
        if(x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        for(int x = 0; x <= abs(dx); x++) 
        {
            auto _x = x0 + x;
            auto _y = y0 + x * t;
            //std::cout << _x << ":" << _y << std::endl;
            bmp.putPixel(_x, _y, color);
        }
    }
    else
    {
        if(y0 > y1)
        {
            std::swap(y0, y1);
            std::swap(x0, x1);
        }
        for(int y = 0; y <= abs(dy); y++) 
        {
            auto _y = y0 + y;
            auto _x = x0 + y / t;
            if(x1 == x0)
                _x = x0;
            //std::cout << _x << ":" << _y << std::endl;
            bmp.putPixel(_x, _y, color);
        }
    }
}



struct Vec3f
{
    Vec3f(float x, float y, float z)
    {
        verx.push_back(x);
        verx.push_back(y);
        verx.push_back(z);
    }
    float& x()
    {
        return verx[0];
    }
    float& y()
    {
        return verx[1];
    }
    float& z()
    {
        return verx[2];
    }

private:
    std::vector<float> verx;
};

void trim(std::string &str)
{
    auto ait = str.begin();
    auto eit = str.end();
    while(*ait++ == ' ')
        str.erase(ait);

    while(*--eit == ' ')
        str.erase(eit);
}

std::vector<std::string> split(std::string str, std::string delim = " \t")
{
    std::vector<std::string> ret;
    std::size_t index =  0;
    std::size_t prev = 0;
    while((index = str.find_first_of(delim, index)) != std::string::npos)
    {
        auto st = std::string(str, prev, index - prev);
        if(st != "")
            ret.push_back(st);
        index++;
        prev = index;
    }
    auto st = std::string(str, prev, str.size() - prev);
    ret.push_back(st);
    return ret;
}

struct  Model
{
    Model(std::string name)
    {
        std::ifstream file(name);
        std::string line;
        double max_x = 0;
        double max_y = 0;
        double max_z = 0;
        while(std::getline(file, line))
        {
            trim(line);
            auto vals = split(line); 
            char c = line[0];
            if(c == '#')
                continue;

            if(c == 'f')
            {
                int fcs[3] = { 0 };
                std::stringstream ss;
                for(int i = 1; i < 4; i++)
                {
                    ss << split(vals[i], "/")[0];
                    ss >> fcs[i-1];
                    ss.clear();
                }
                Vec3f vec(fcs[0], fcs[1], fcs[2]);
                faces.push_back(vec);
            }

            if(c == 'v')
            {
                double cc[3] = { 0 };
                std::stringstream ss;
                for(int i = 0; i < 3; i++)
                {
                    ss << vals[i+1];
                    ss >> cc[i];
                    ss.clear();
                }
                if(abs(cc[0]) > max_x)
                    max_x = abs(cc[0]);
                if(abs(cc[1]) > max_y)
                    max_y = abs(cc[1]);
                if(abs(cc[2]) > max_z)
                    max_z = abs(cc[2]);

                Vec3f vec(cc[0], cc[1], cc[2]);
                vertices.push_back(vec);
            }
        }
        //std::cout << max_x << " " << max_y << std::endl;
        max_x++;
        max_y++;
        ratio = max_x / max_y;
        for(auto &it : vertices)
        {
            it.x() /= max_x;
            it.y() /= max_y;
            it.z() /= max_z;
        }

        file.close();
    }

    void operator>>(Bitmap &bmp)
    {
        for(auto & it : faces)
        {
            Vec3f &vec0 = vertices[it.x()-1];
            Vec3f &vec1 = vertices[it.y()-1];
            Vec3f &vec2 = vertices[it.z()-1];

            //std::cout << "Passing ";
            //std::cout << vec0.x() << ":" << vec0.y() << std::endl;
            //std::cout << vec1.x() << ":" << vec1.y() << std::endl;
            //std::cout << vec2.x() << ":" << vec2.y() << std::endl;
            auto x0 = bmp.Width() / 2 + vec0.x() * bmp.Width() / 2 * ratio;
            auto y0 = bmp.Height() / 2 + vec0.y() * bmp.Height() / 2;
            auto x = bmp.Width() / 2 + vec1.x() * bmp.Width() / 2 * ratio;
            auto y = bmp.Height() / 2 + vec1.y() * bmp.Height() / 2;
            auto x1 = bmp.Width() / 2 + vec2.x() * bmp.Width() / 2 * ratio;
            auto y1 = bmp.Height() / 2 + vec2.y() * bmp.Height() / 2;

            line(bmp, x0, y0, x, y, 0xF000);
            line(bmp, x, y, x1, y1, 0xF000);
            line(bmp, x0, y0, x1, y1, 0xF000);
        }
        bmp.commit("file.bmp");
    }
    std::vector<Vec3f> vertices;
    std::vector<Vec3f> faces;
    float ratio;
};

int main(int argc, char **argv)
{
    auto const H = 800;
    auto const W = 800;
    Bitmap bmp(W, H);
    Model model("head.obj");
    //Model model("Stalin_Bust_OBJ_reduced.obj");
    model >> bmp;
}
