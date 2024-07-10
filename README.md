# OpenGLDeferredRendering

This loads OpenGL deferred rendering pipeline loads blend OBJ files to mostly recreate a scene. A deferred rendering pipeline is utilized to implement screen space ambient occlusion. 


![image](https://github.com/EmmyVoita/OpenGLDeferredRendering/assets/82542924/e2fbda9c-671f-43e6-b917-c1c448d5d951)


To run this program you will need everything in the repository. 
If anything is missing the program will throw an error or a segmentation fault will occur. 

To compile, navigate to the download file and run the following command in an application that runs Linux like Ubuntu:

g++ DeferredRendering.cpp -o run -lglfw -lGL -lGLEW -lSOIL

How to run:

./run

