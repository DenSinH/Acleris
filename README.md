# Acleris
A software rasterizer with nice syntax.

The main goal of the project was to learn how rasterization work, and to try to do it in a fast way. Another goal was to use my vector math wrapper (see [here](https://github.com/DenSinH/VMath)).

The syntax turned out quite nice. Initialization of the program would be something like 

```cxx
Acleris rasterizer(800, 800);
rasterizer.Projection(0.1, 0.1, 0.1, 100.0);

DrawList list(rasterizer);
```

Then when calling `rasterizer.SDLRun(...)` you can pass a lambda that possibly takes an `Acleris::Mouse` and/or an `Acleris::Keyboard` argument (it figures out what 
it needs at compile time), and then write your body. So I could for example do 
```cxx
rasterizer.SDLRun([&](Acleris::Mouse mouse, Acleris::Keyboard keyboard) {
  ...
});
```
but
```cxx
rasterizer.SDLRun([&]() {
  ...
});
```
or
```cxx
rasterizer.SDLRun([&](Acleris::Keyboard keyboard) {
  ...
});
```
work as well. Then you should probably clear the screen with `rasterizer.Clear();` and you can start drawing geometry with code like
```cxx
// we can pass more vertex parameters by adding them to the constructor, for example
// auto example = MakeVertex<3>({-0.5, 0.5, 0}, RGB(1.0, 0.0, 0.0), v2{1, 0}, 1.0f);
// to pass some other parameters
// the fragment shader can then use those (interpolated) arguments by adding them as arguments (in the right order), like
// .Fragment([&](Color c, v2 tex, float value) {....});
// which we will see later

auto vert0 = MakeVertex<3>({-0.5, 0.5, 0}, RGB(1.0, 0.0, 0.0));
auto vert1 = MakeVertex<3>({ 0.5, 0.5, 0}, RGB(0.0, 1.0, 0.0));
auto vert2 = MakeVertex<3>({   0,-0.5, 0}, RGB(0.0, 0.0, 1.0));
auto vert3 = MakeVertex<3>({   0, 0, 0.5}, RGB(1.0, 0.0, 1.0));

m3x3 mat{
    {std::cos(t), std::sin(t), 0},
    {-std::sin(t), std::cos(t), 0},
    {0, 0, 1},
};

for (int i = 0; i < 50; i++) {
    list << (mat * Triangle(vert0, vert1, vert2) + v3{i, 0, 0}).Fragment([](const Color& c) {
        // you can add more interesting code here...
        return c;
    });
    list << (mat * Triangle(vert0, vert1, vert3) + v3{i, 0, 0}).Fragment([](const Color& c) {
        return c;
    });
    list << (mat * Triangle(vert0, vert2, vert3) + v3{i, 0, 0}).Fragment([](const Color& c) {
        return c;
    });
    list << (mat * Triangle(vert1, vert2, vert3) + v3{i, 0, 0}).Fragment([](const Color& c) {
        return c;
    });
}

// drawing a solid color with .Color(...) instead
list << Line(MakeVertex<3>(v3{-1, 0, 0}), MakeVertex<3>(v3{1, 0, 0})).Color(RGB(1, 0, 0));
list << Line(MakeVertex<3>(v3{0, -1, 0}), MakeVertex<3>(v3{0, 1, 0})).Color(RGB(0, 1, 0));
list << Line(MakeVertex<3>(v3{0, 0, -1}), MakeVertex<3>(v3{0, 0, 1})).Color(RGB(0, 0, 1));

list << Point(MakeVertex<3>(v3{2, 2, 0})).Color(0xffff'ffff);

list.Wait();
```
The fragment shading here can be completely changed in any way you like (this was the main goal). You can also pass more varying parameters, simply by adding them 
to the vertex constructors (`vert0`, etc.). The compiler will inspect the parameters in the vertices, and the arguments to the fragment shader and try to match them
(so passing too many is no problem, passing too few will break the program).

They are placed in a drawlist where the triangles are drawn asynchronously, and you can wait until they are drawn before you return from the screen update function.

A video of the example code being run can be seen below:

https://user-images.githubusercontent.com/25347040/144504131-3bb75b8c-48d2-4130-8e87-fa28719d248e.mp4
