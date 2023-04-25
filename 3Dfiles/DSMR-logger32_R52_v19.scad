//---------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for a DSMR-logger32 v5.2 PCB
//
//  Version 1.0 (25-04-2023)
//
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------


//-- these parms need to be declared before including the YAPPgenerator
//

//-- default is 0.9" oled -----
oled_13_inch          = false;

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
oledHeader09X       = 16;       //       +~~~~~~~~~~~~~~~+            
oledHeader09Y       = 40;       //       |     ==o==     | HeaderX    ^
oledPcb09Width      = 28;       //       +---------------+ ScreenXs   |
oledPcb09Height     = 19;       //       |               |            |
oledScreen09Xs      =  1.5;     //       |               |            > Height
oledScreen09Xe      = 16.5;     //       |_______________|            |
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


include <./YAPP_Box/library/YAPPgenerator_v19.scad>

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
printOledStand        = true;
printSwitchCap        = true;

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
standoffHeight      = 4.0;
pinDiameter         = 2.0;
pinHoleSlack        = 0.3;
standoffDiameter    = 5;

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
showSideBySide      = true;
hideLidWalls        = false;
onLidGap            = 0;
shiftLid            = 4;
colorLid            = "yellow";
hideBaseWalls       = false;
colorBase           = "white";
showOrientation     = true;
showPCB             = false;
showMarkers         = false;
inspectX            = 0;  // 0=none, >0 from front, <0 from back
inspectY            = 0;  // 0=none, >0 from left, <0 from right


//-- pcb_standoffs  -- origin is pcb[0,0,0]
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
      [14, 6,  6.5, 6.5, 0, yappCircle, yappCenter]     // Reset Button
     ,[41, 6,  8, 8, 0, yappRectangle, yappCenter]      // Flash Button
     //-- if no OLED screen remove next line
     ,[oledScreenXs+oledHeaderX, oledHeaderY-(oledScreenWidth/2)
                        , oledScreenWidth, oledScreenHeight
                        , 0, yappRectangle]  // OLED
     ,[4, 18, 6, 6, 0, yappRectangle, yappCenter]       // NeoPixel 1
     ,[4, 28, 6, 6, 0, yappRectangle, yappCenter]       // NeoPixel 2
     ,[4, 40, 6, 6, 0, yappCircle]                      // NeoPixel 3
   //,[pcbLength-16, 37.5, 15, 18, 0,  yappRectangle]   // RJ12
   //,[pcbLength-16, 56.0, 15, 18, 0,  yappRectangle]   // RJ12
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
  wallThickness = 1;
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
      translate([rimXout,rimYout,-1])
      {
        color("red") cube([oledScreenHeight+rimWidth,oledScreenWidth+rimWidth,2.5]);
      }
      translate([rimXin,rimYin,-2])
      {
        color("blue") cube([oledScreenHeight, oledScreenWidth, 4]);
      }
    }
    
    //-- side wall's
    difference()
    {
      translate([wallXout,wallYout,-4])
      {
        color("green") cube([wallOutH,wallOutW,6]);
      }
      translate([wallXin-1,wallYin,-4.5]) 
      {
        color("blue") cube([wallInH+1, wallInW, 7.5]);
      }
    }
  }
  
} //  insideOledScreen()



//----------------------------------------------------------------------------
module insideSwitch(isReset, x, y, s, d, topPcb)
{
                            //          <-outside-->
                            //           >-------< -- (s)
  rX            = pcbX+x;   //        -+-+       +-+--  ^
  rY            = pcbY+y;   //         | |       | |    | bH
  inside        = 4.5;      //         |  +-+  +-+ |    v
  outsideFlash  = s+2;      //         +--+ | | +--+
  outsideReset  = s+2;      //        ----+-+ +-+----
  bH            = 2+d;
  wall          = 3.0;
  //outside = s+2;
  echo("insideSwitch():",pcbX=pcbX,x=x,rX=rX,pcbY=pcbY,y=y,rY=rY);

  translate([rX, rY, (topPcb*-1)-0.5])
  {
      if (isReset)
      {
        difference()
        {
          union()
          {
            translate([outsideReset/-2,outsideReset/-2,topPcb-bH]) 
            {
              color("red") 
                translate([outsideReset/2,outsideReset/2,0])
                  cylinder(h=bH, d=outsideReset);
            }
            translate([0, 0, 4])
            {
              color("gray") cylinder(d=inside+wall, h=(topPcb-5));
            }
          } //-- union()
          
          translate([(s/-2),(s/-2),topPcb-d])
          {
            translate([s/2, s/2, 0])
              //color("blue") cylinder(h=5, d=s/1.35);
              color("blue") cylinder(h=5, d=6.5);
          }
          translate([0, 0, 15/2]) 
          {
            color("orange") cylinder(d=inside, h=15, center=true);
          }
        } // difference()
      }
      else
      {
        difference()
        {
          union()
          {
            translate([outsideFlash/-2,outsideFlash/-2,topPcb-bH]) 
            {
              color("red") 
                roundedCube([outsideFlash,outsideFlash, bH], false, radius = 1.0, apply_to = "");
            }
            translate([0, 0, 4])
            {
              color("gray") cylinder(d=inside+wall, h=(topPcb-5));
            }
          } //-- union()
          
          translate([(s/-2),(s/-2),topPcb-d])
          {
            color("blue") cube([s,s,5]);
          }
          translate([0, 0, 15/2]) 
          {
            color("orange") cylinder(d=inside, h=17, center=true);
          }
        } //-- difference()
    } //--  if .. else
  } //-- translate()
  
} //  insideSwitch()


//----------------------------------------------------------------------------
module insideNeoTube(isWatchdog, x, y, topPcb)
{
  translate([x, y, (topPcb/-2)])
  {
    difference()
    {
      color("red") cube([9,9,topPcb], true);
      if (isWatchdog)
      {
        translate([0,0,-0.6]) color("blue") cylinder(h=topPcb, d=6, center=true);
      }
      else
      {
        translate([0,0,-0.6]) color("blue") cube([6,6,topPcb], true);
      }
    }
  }

} //  insideNeoTube()


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
        polygon(points=[[0,0],[0,23],[9,23]]);
      
  topPCB = (baseWallHeight+lidWallHeight)-(standoffHeight+2);
  //-- [14,  6,  8, 8, 0, yappRectangle, yappCenter]  //  [Reset] button
  // (0) = posx
  // (1) = posy
  // (2) = width
  // (3) = z-ax
  //-- [Reset] button
  insideSwitch(true, 14, 6, 8, 4.5, topPCB);
  
  //-- [Flash] Button
  //-- [41,  6,  8, 8, 0, yappRectangle, yappCenter]    // [Rlash] Button
  insideSwitch(false, 41, 6, 8, 2.5, topPCB);

  //-- NeoPixel
  //  [x=4, y=18]
  insideNeoTube(false, pcbX+4, pcbY+18, topPCB);
  //-- NeoPixel
  //  [x=4, y=28]
  insideNeoTube(false, pcbX+4, pcbY+28, topPCB);

  //-- NeoPixel [WatchDog]
  //  [x=4, y=40]
  insideNeoTube(true, pcbX+4, pcbY+40, topPCB);
  //-- OLED screen
  if (printInsideOLED) insideOledScreen(topPCB);
  
} //  lidHookInside()


//-- switch extender -----------
if (printSwitchExtenders)
{
  extH        = -1.5;
  poleDiam    = 4;
  //--poleHeight1 = 16; 
  poleHeight1 = (baseWallHeight + lidWallHeight) -5;
  //--poleHeight2 = 14; 
  poleHeight2 = (baseWallHeight + lidWallHeight) -7;
  feetDiam    = 7.5;
  feetHeight  = 2;  //1;

  zeroExtend=shellHeight - (standoffHeight + basePlaneThickness + pcbThickness);
  echo(zeroExtend=zeroExtend);
     
  //-- switch extender [Flash] button
  translate([-10,75,0])
  {
    translate([0, 0, (poleHeight1/2)]) 
          cylinder(d=poleDiam,h=(poleHeight1-feetHeight), center=true);  // above shell
    translate([0, 0, (feetHeight/2)]) 
          cylinder(d=feetDiam, h=feetHeight, center=true);
  }
  //-- switch extender [Reset] button
  translate([-10,55,0])
  {
    translate([0, 0, (poleHeight2/2)]) 
          cylinder(d=poleDiam,h=(poleHeight2-feetHeight), center=true);  // above shell
    translate([0, 0, (feetHeight/2)]) 
          cylinder(d=feetDiam, h=feetHeight, center=true);
  }
    
} // .. printSwitchExtenders?


//-- switch Cap -----------
if (printSwitchCap)
{                         //      <------> ---- capRib / capDiam
  capRib      = 7.4;      //      +-+  +-+ ----------------------
  capDiam     = 5.8;      //      | |__| |                ^
                          //      |      | ^              |-- capHeight
  capHeight   = 4;        //      |      | +-- topHeight  v 
  stingDiam   = 4.2;      //      +------+ v --------------------
  topHeight   = 1;        //        >--<------ stingRib
    //--- cap 1 [Reset] ---
    translate([-15,110,0])
    {
      difference()
      {
        translate([0,0,(capHeight/2)]) color("red")
          //cylinder(h=capHeight, d=capDiam, center=true);
            cylinder(h=capHeight, d1=capDiam-0.5, d2=capDiam, center=true);
        translate([0, 0, (capHeight/2)+topHeight]) color("blue")
            cylinder(d=stingDiam, h=capHeight, center=true);
      }
    }

    //--- cap 2 [Flash] ---
    translate([-15,95,0])
    {
      difference()
      {
        translate([0,0,(capHeight/2)]) color("red")
            roundedCube([capRib, capRib, capHeight], true, radius = 1.0, apply_to = "all");
        translate([0, 0, (capHeight/2)+topHeight]) color("blue")
            cylinder(d=stingDiam, h=capHeight, center=true);
      }
    }
    
} // .. printSwitchCap?


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
    else  //-- 0.95"
    {
      //-- oled Stand 0.95"
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
