//---------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for a DSMR-logger32 v5.2 PCB
//
//  Version 1.0 (12-07-2023)
//
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------


//-- these parms need to be declared before including the YAPPgenerator
//

//-- select oled size (both 'false' for none) -----
oled_09_inch          = false;
oled_13_inch          = false;

if (oled_09_inch && oled_13_inch)
{
  for (i = [0 : 1 : 20])
  {
    echo("====================================");
    echo("Select (only one!) OLED screen size");
    echo("====================================");
  }
}
//
//- these dimensions are for a 1.3" OLED display
//-- oled dimensions and Header position
//                              //               V-------- HeaderY
oledHeader13X       = 16;       //       +~~~~~~~~~~~~~~~+            
oledHeader13Y       = 40;       //       |     ==o==     | HeaderX    ^
oledPcb13Width      = 37;       //       +---------------+ ScreenXs   |
oledPcb13Height     = 24;       //       |               |            |
oledScreen13Xs      =  3;       //       |               |            > Height
oledScreen13Xe      = 26;       //       |_______________|            |
                                //       +---------------+ ScreenXe   |
                                //       |               |            |
                                //       +---------------+            v
                                //       <--- Width ----->


//-- these dimensions are for a 0.9" OLED display
//-- oled dimensions and Header position
//                              //               V-------- HeaderY
oledHeader09X       = 17;       //       +~~~~~~~~~~~~~~~+            
oledHeader09Y       = 40;       //       |     ==o==     | HeaderX    ^
oledPcb09Width      = 28;       //       +---------------+ ScreenXs   |
oledPcb09Height     = 19;       //       |               |            |
oledScreen09Xs      =  1.5;     //       |               |            > Height
oledScreen09Xe      = 19.0;     //       |_______________|            |
                                //       +---------------+ ScreenXe   |
                                //       |               |            |
                                //       +---------------+            v
                                //       <--- Width ----->



oledScreen09Width  = oledPcb09Width;
oledScreen09Height = oledScreen09Xe - oledScreen09Xs;
oledScreen09X      = oledHeader09X + oledScreen09Xs;
oledScreen09Y      = oledHeader09Y - (oledScreen09Width/2);

oledScreen13Width  = oledPcb13Width;
oledScreen13Height = oledScreen13Xe - oledScreen13Xs;
oledScreen13X      = oledHeader13X + oledScreen13Xs;
oledScreen13Y      = oledHeader13Y - (oledScreen13Width/2);

//-- normalize params -----
oledHeaderX       = (oled_13_inch) ? oledHeader13X : oledHeader09X;
oledHeaderY       = (oled_13_inch) ? oledHeader13Y : oledHeader09Y;
oledScreenXs      = (oled_13_inch) ? oledScreen13Xs : oledScreen09Xs;
oledScreenXe      = (oled_13_inch) ? oledScreen13Xe : oledScreen09Xe;
oledScreenWidth   = (oled_13_inch) ? oledScreen13Width : oledScreen09Width;
oledScreenHeight  = (oled_13_inch) ? oledScreen13Height : oledScreen09Height;

oledRecess = (oled_09_inch || oled_13_inch) ? [oledScreenXs+oledHeaderX, oledHeaderY-(oledScreenWidth/2)
                      , oledScreenWidth, oledScreenHeight
                      , 0, -1] //-- "-1" == yappRectangle
                      : [];



include <./YAPP_Box/library/YAPPgenerator_v20.scad>

include <./YAPP_Box/library/roundedCubes.scad>


/*
see https://polyd.com/en/conversione-step-to-stl-online
*/

myPcb = "./STL/MODELS/DSMR-logger32_MODEL_R52.stl";

if (false)
{
  translate([-47.3,-157.5,4.6]) 
  {
    rotate([0,0,90]) color("white") import(myPcb);
  }
}


// Note: length/lengte refers to X axis, 
//       width/breedte to Y, 
//       height/hoogte to Z

/*
      padding-back|<------pcb length --->|<padding-front
                            RIGHT
        0    X-as ---> 
        +----------------------------------------+   ---
        |                                        |    ^
        |                                        |   padding-right 
        |                                        |    v
        |    -5,y +----------------------+       |   ---              
 B    Y |         | 0,y              x,y |       |     ^              F
 A    - |         |                      |       |     |              R
 C    a |         |                      |       |     | pcb width    O
 K    s |         |                      |       |     |              N
        |         | 0,0              x,0 |       |     v              T
      ^ |   -5,0  +----------------------+       |   ---
      | |                                        |    padding-left
      0 +----------------------------------------+   ---
        0    X-as --->
                          LEFT
*/

printBaseShell        = true;
printLidShell         = true;
printSwitchExtenders  = true;
printInsideOLED       = true;
printOledStand        = false;

// Edit these parameters for your own board dimensions
wallThickness       = 2.4;
basePlaneThickness  = 1.0;
lidPlaneThickness   = 1.2;

//-- total height = 14 + pcbThickness + standoffHeight
//--         19.6 = 14 + 1.6 + 4
baseWallHeight      = 13; 
lidWallHeight       = 11; //-- was 8;

// ridge where base and lid off box can overlap
// Make sure this isn't less than lidWallHeight
ridgeHeight         = 3;
ridgeSlack          = 0.2;
roundRadius         = 1.0;

// How much the PCB needs to be raised from the base
// to leave room for solderings and whatnot
standoffHeight      = 4.0;  //-- only used to position PCB
standoffDiameter    = 5;
standoffPinDiameter = 2.0;
standoffHoleSlack   = 0.4;


// Total height of box = basePlaneThickness + lidPlaneThickness 
//                     + baseWallHeight + lidWallHeight
pcbLength           = 77;
pcbWidth            = 80;
pcbThickness        = 1.7;
                            
// padding between pcb and inside wall
paddingFront        = 1;
paddingBack         = 1;
paddingRight        = 1;
paddingLeft         = 1;


//-- D E B U G -------------------
showSideBySide      = true;     //-> true
onLidGap            = 0;
shiftLid            = 1;
hideLidWalls        = false;    //-> false
hideBaseWalls       = false;    //-> false
showOrientation     = true;
showPCB             = false;
showSwitches        = false;
showPCBmarkers      = false;
showShellZero       = false;
showCenterMarkers   = false;
inspectX            = 0;        //-> 0=none (>0 from front, <0 from back)
inspectY            = 0;        //-> 0=none (>0 from left, <0 from right)
inspectLightTubes   = 0;        //-> { -1 | 0 | 1 }
inspectButtons      = 0;        //-> { -1 | 0 | 1 }
//-- D E B U G -------------------


//-- pcb_standoffs  -- origin is pcb[0,0,0]
//-- uses "standoffXYZ" values
// (0) = posx
// (1) = posy
// (2) = standoffHeight
// (3) = flangeHeight
// (4) = flangeDiam
// (5) = { yappBoth | yappLidOnly | yappBaseOnly }
// (6) = { yappHole, YappPin }
// (7) = { yappAllCorners | yappFrontLeft | yappFrondRight | yappBackLeft | yappBackRight }
pcbStands = [
                [ 5, 4, 4, 3, 6, yappBoth, yappFrontRight]
              , [ 5, 5, 4, 4, 8, yappBoth, yappBackLeft]
            ];     

//-- Lid plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLid =  [
                oledRecess
              ];

//-- base plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBase =   [];

//-- cutoutGrills    -- origin is pcb[x0,y0,zx]
// (0) = xPos
// (1) = yPos
// (2) = grillWidth
// (3) = grillLength
// (4) = gWidth
// (5) = gSpace
// (6) = gAngle
// (7) = plane {"base" | "lid" }
// (8) = {polygon points}}

cutoutsGrill = [
                 [22, 30, 20, 30, 2, 3,  45, "base"]
               ];
              
//-- front plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsFront =  [
                    [37.5, -0.5, 15, 17, 0, yappRectangle]  // RJ11
                  , [56.0, -0.5, 15, 17, 0, yappRectangle]  // RJ11
                  , [23.5,  6,   11, 11, 0, yappCircle]     // PWR Jack

                ];

//-- back plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBack =   [
                ];

//-- left plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLeft =   [
                ];

//-- right plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsRight =  [
                ];

//-- connectors 
//-- normal         : origen = box[0,0,0]
//-- yappConnWithPCB: origen = pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = pcbStandHeight
// (3) = screwDiameter
// (4) = screwHeadDiameter
// (5) = insertDiameter
// (6) = outsideDiameter
// (7) = flangeHeight
// (8) = flangeDiam
// (9) = { yappConnWithPCB }
// (10) = { yappAllCorners | yappFrontLeft | yappFrondRight | yappBackLeft | yappBackRight }
connectors   =  [
                   [ 5, 5, 4, 2.9, 5.2, 4.1, 4, 3, 12, yappConnWithPCB, yappFrontLeft, yappBackRight]
                ];


//-- base mounts -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = screwDiameter
// (2) = width
// (3) = height
// (4..7) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (5) = { yappCenter }
baseMounts   = [
                    [10, 3.5, 15, 3, yappRight, yappCenter]
                  , [shellLength-25, 3.5, 15, 3, yappLeft, yappCenter]
               ];
               
//-- snap Joins -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = width
// (2..5) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (n) = { yappSymmetric }
snapJoins   =  [
                  [36, 3, yappFront]
                , [78, 4, yappFront]
                , [15, 4, yappLeft]
                , [70, 4, yappRight]
                , [15, 4, yappBack]
               ];
               
//-- lightTubes  -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = tubeLength
// (3) = tubeWidth
// (4) = tubeWall
// (5) = abovePcb
// (6) = througLid {yappThroughLid}
// (7) = tubeType  {yappCircle|yappRectangle}
lightTubes = [
             //-- 0,  1, 2, 3, 4, 5, 6/7
             //  [4, 18, 6, 3, 1, 2, yappRectangle, yappThroughLid]       // NeoPixel 1
             // ,[4, 28, 6, 3, 1, 2, yappRectangle, yappThroughLid]       // NeoPixel 2
             // ,[4, 42, 6, 6, 1, 2, yappCircle, yappThroughLid]          // NeoPixel 3
                 [4, 18, 6, 3, 1, 2, yappRectangle]                 // NeoPixel 1
                ,[4, 28, 6, 3, 1, 2, yappRectangle]                 // NeoPixel 2
                ,[4, 42, 6, 6, 1, 2, yappCircle]                    // NeoPixel 3
              ];     

//-- pushButtons  -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = capLength
// (3) = capWidth
// (4) = capAboveLid
// (5) = switchHeight
// (6) = switchTrafel
// (7) = poleDiameter
// (8) = buttonType  {yappCircle|yappRectangle}
pushButtons = [
              //-- 0, 1,  2,   3,   4, 5,   6, 7,   8
                 [14, 6,  6.5, 6.5, 0, 2, 1, 3.5, yappCircle]       // Reset Button
                ,[41, 6,  4,   8,   2, 2, 1, 3.5, yappRectangle]    // Flash Button
              ];     
                            
//-- origin of labels is box [0,0,0]
// (0) = posx
// (1) = posy/z
// (2) = orientation
// (3) = depth
// (4) = plane {lid | base | left | right | front | back }
// (5) = font
// (6) = size
// (7) = "label text"
labelsPlane =  [
                  [19,  2, 0, 1, "front", "Liberation Mono:style=bold", 3, "5 Volt" ]
                , [46,  2, 0, 1, "front", "Liberation Mono:style=bold", 3, "P1" ]
                , [65,  2, 0, 1, "front", "Liberation Mono:style=bold", 3, "S1" ]
               ];

               
//----------------------------------------------------------------------------
module insideOledScreen(topPCB)
{
  zVal          = (baseWallHeight+lidWallHeight)-(standoffHeight-pcbThickness)-1;
  zeroX         = pcbX+oledHeaderX;
  zeroY         = pcbY+oledHeaderY;
  
  rimWidth      = 2;
  rimXin        = oledScreenXs;
  rimYin        = (oledScreenWidth/2)*-1;
  rimXout       = rimXin-(rimWidth/2);
  rimYout       = rimYin-(rimWidth/2);
  wallFromRim   = 2.5;
  wallThickness = 2;
  wallInW       = oledScreenWidth+wallFromRim;
  wallInH       = oledScreenHeight+8;
  wallXin       = 0; //oledScreenXs-(wallFromRim/2);
  wallYin       = wallInW/-2;
  wallOutW      = oledScreenWidth+wallThickness+wallFromRim;
  wallOutH      = oledScreenHeight+6;
  wallXout      = 0;
  wallYout      = wallOutW/-2;
  
  echo(zeroX=zeroX, zeroY=zeroY);
  echo(rimXin=rimXin, rimYin=rimYin, rimYout=rimXout, rimYout=rimYout);
  echo(wallXin=wallXin, wallYin=wallYin, wallXout=wallXout, wallYout=wallYout);
  
  translate([zeroX, zeroY, (topPCB-zVal)])
  {
    //-- reference center of oled Screen Header
    //color("black") cylinder(r=1, h=10, center=true);
    
    //-- rim
    difference()
    {
      translate([rimXout,rimYout,-3])
      {
        color("red") cube([oledScreenHeight+rimWidth,oledScreenWidth+rimWidth,5.5]);
      }
      //-19-translate([rimXin,rimYin,-2])
      translate([rimXin,rimYin,-3.5])
      {
        //-19-color("white") cube([oledScreenHeight, oledScreenWidth, 4]);
        color("white") cube([oledScreenHeight, oledScreenWidth, 7]);
      }
    }
    
    //-- side wall's
    difference()
    {
      //-19-translate([wallXout,wallYout,-4])
      translate([wallXout,wallYout,-7])
      {
        //-19-color("green") cube([wallOutH,wallOutW,6]);
        color("green") cube([wallOutH,wallOutW,9]);
      }
      //-19-translate([wallXin-1,wallYin,-4.5]) 
      translate([wallXin-1,wallYin,-7.5]) 
      {
        color("blue") cube([wallInH+1, wallInW, 10.5]);
      }
    }
    
  }
  
} //  insideOledScreen()



module baseHookOutside()
{
} //  baseHookOutside()


module baseHookInside()
{
} //  baseHookInside()


module lidHookOutside()
{
  
} //  lidHookOutside()


module lidHookInside()
{
  //-- strengthening between RJ12's --
  translate([shellLength-24,58.5,0])
    rotate([90,90,0])
      linear_extrude(1.8)
        polygon(points=[[0,0],[0,23],[12,23]]);
      
  topPCB = (baseWallHeight+lidWallHeight)-(standoffHeight+2);

  if (printInsideOLED) insideOledScreen(topPCB);
  
} //  lidHookInside()


//-- oled Stand -----------
if (printOledStand)
{
    standHeight = 11;
  
    if (oled_13_inch)
    {
      //-- oled Stand 1.3"
      translate([-15,125,0])
      {
        translate([1,0,0])      cube([2,36, 2]);
        translate([-1,5,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,0,0])     color("blue")   cube([3,8,2]);  // 
        translate([-2.5,2,0])   color("black")  cube([4,1.5,1.5]);  // centreer nop
        translate([-1,28,0])    color("green")  cube([standHeight,3,2]); // stand
        translate([-1,33-5,0])  color("blue")   cube([3,8,2]);  //
        translate([-2.5,32,0])  color("red")    cube([4,1.5,1.5]);  // centreer nop
        //translate([-3,3,0]) cube([1,30,1]);
      }
    }
    else  //-- 0.96"
    {
      //-- oled Stand 0.96"
      translate([-15,125,0])
      {
        translate([1,0,0])      cube([2,oledScreenWidth-2, 2]);
        translate([-1,0,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,0,0])     color("blue")   cube([3,8,2]);  // 
        translate([-2.5,2,0])   color("black")  cube([4,1.5,1.5]);  // centreer nop
        translate([-1,oledScreenWidth-2,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,oledScreenWidth-7,0])     color("blue")   cube([3,8,2]);  //
        translate([-2.5,oledScreenWidth-3,0])   color("red")    cube([4,1.5,1.5]);  // centreer nop
        //translate([-3,3,0]) cube([1,30,1]);
      }
      //translate([-3,3,0]) color("black") cube([1,23,1]);
    }
      
    
} // .. printOledStand?


//---- This is where the magic happens ----
YAPPgenerate();
