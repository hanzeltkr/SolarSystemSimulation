(IN PROGRESS)

Solar System Simulation

This project was made to experiment how to integrate topic from physics into graphical display. The simulation used the principle of gravity to calculate how planets interact with each other and the Sun. Furthermore, the simulation receives control from hand-gesture to control how users view the system. With the goal to be as accurate as possible, this is also a practice of dealing and optimizing calculations with large numbers while being able to display results smoothly.

P.S. This is my first project that I try to manage seriously without guidance therefore if some explanation or code are not logical or hard to follow along I sincerely apologise beforehand and feel free to give some feedback on how I can improve. Also this project is set to be done in roughly around a month as I want to try to make something small and new during winter break therefore it is not completely perfect.

How It's Made
Tech : C++, OpenGL, GLSL, Python, OpenCV

1. Create a 3D space for models to exist and camera to render views.
      First I created a viewport with 2D rendering first. I have created a 3D space by including camera with model matrix, view matrix, and projection matrix for to recieve input simultaneously from the world space coordinates. 
2. Create Planets class.
      To keep things simple and straight forward, I made all planets a sphere. To create them, I reference Sphere.h and Sphere.cpp from _____ as they have already created class for constructing and drawing Spheres. The class included the vertices and indices function which are neccessary information to draw them. 
      Then I createed Planet class that stores all the information of each members of the Solar System : name, mass, radius, distance from the Sun (meters), orbital period (Earth years), color, and inclination. For the planet constructor, each information is assigned for each planet with randomize starting point of its orbit. The orbital velocity of the planet is calculated using v = sqrt(GM/r) which is from a = sqrt(GMm/r^2). 
      There are some exceptions : for the Sun, it is scaled more than other planets as with same scaling as planets, it is impossible to locate planets' location with how relatively small they are (proving how big the Sun actually is). Also, its position is set to the origin with velocity as 0.
      planetSetup is a method that assigns vertices and indices information to vertex array object (VAO) using vertex buffer object (VBO), a buffer for vertices vboId and a buffer for indices iboId. Set up strides for the VAO and unbind them.
      draw method takes in a shader which is created outside this class to access the vertex and fragment shader and draw them out according to their current position which is updated with uniform input for every render.
      getGravitationalForce is a method dedicated to calculate the gravitational forces that a planet receives from all other planets. This is the part where I use F = GMm/r^2 and add each F from each planets together to calculate the final force.
      update is a method that updates planets position depending on the time speed. This part is very important as I discovered along the way as I was experimenting with speeding up simulation. It turns out that the more dt that occurs when time is sped up (especially in such large scale as x days per second), the larger inaccuracy became as velocity and position gets updated. Therefore, the idea of dividing dt into smaller substep was brought up to calculate with smaller dt but updating it more frequently within the amount of time we have with setting the upper limit for dt. If dt is small enough that it doesn't need substep then it can fully be used so this process doesn't need to be done when speeding doesn't create too much inaccuracy. The substep process was done in updateSingleStep.
