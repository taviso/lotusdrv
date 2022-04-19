--**************************************************************************
--
--   Copyright (C) 1991-1994 Lotus Development Corporation.
--   All rights reserved.
--
--   1.  Lotus grants you a non-exclusive royalty-free right to use and modify
--       the source code version and to reproduce and distribute the object
--       code version of the Example Programs provided in the Toolkit, provided
--       that you:
--
--       a) distribute the Example Programs only in conjunction with and as a
--          part of your software application product which is designed as an
--          add-in to Lotus 1-2-3, and
--
--       b) do not represent your product to be a product of Lotus Development
--          Corporation.
--
--   2.  The Example Programs are provided as is, without warranty of any kind,
--       either express or implied, including, without limitation, the implied
--       warranties of merchantability of fitness for a particular purpose.
--
--   File Name:  measure.pl
--
--**************************************************************************

module measure is

use Register    -- the module containing the Register procedure

  Register("Area", At_Function)
     except on others do
       Print("Unable to register @AREA")
       end except
  Register("Circum", At_Function)
     except on others do
       Print("Unable to register @CIRCUM")
       end except
  Register("TArea", At_Function)
     except on others do
       Print("Unable to register @TAREA")
       end except
  Register("Volume", At_Function)
     except on others do
       Print("Unable to register @VOLUME")
       end except
  Register("CVolume", At_Function)
     except on others do
       Print("Unable to register @CVOLUME")
       end except

-- Area calculates the area of a rectangle.
-- Inputs: 2 floats
-- Returns: a float
procedure Area (float Length, Width) returns (float A)
  signals (Err)
  purpose ("Calculate area of a rectangle") is

 begin
  if Length <= 0 | Width <= 0 then    -- check for valid input
    signal Err                       -- signal ERR if not valid
  else
    A = Length * Width                -- else return the area
  end if
 end
  except on others (string SignalName) do
    Print("Signal ", SignalName, " raised.")
    end except

 end procedure Area

-- Circum calculates the circumference of a circle.
-- Inputs: a float (the radius of the circle)
-- Returns: a float (the circumference)
procedure Circum (float Radius) returns (float C)
  signals (Err)
  purpose ("Calculate circumference of a circle") is

 begin
  if Radius <= 0 then     -- check for valid input
    signal Err            -- signal ERR if not valid
  else
    C = 2* Pi() * Radius  -- else return circumference
  end if
 end
  except on others (string SignalName) do
    Print("Signal ", SignalName, " raised.")
    end except

 end procedure Circum


-- Volume calculates the volume of a cube.
-- Inputs: 3 floats (Length, Width, Height of cube)
-- Returns: a float (the volume of the cube)
procedure Volume (float Length, Width, Height) returns (float V)
  signals (Err)
  purpose ("Calculate volume of a cube") is

 begin
  if Length <= 0 | Width <= 0 | Height <= 0 then  -- check for valid input
    signal Err                                    -- signal ERR if not valid
  else
    V = Length * Width * Height                   -- else return volume
  end if
 end
  except on others (string SignalName) do
    Print("Signal ", SignalName, " raised.")
    end except

 end procedure Volume


-- TArea calculates the area of a right triangle.
-- Inputs: 2 floats (the base and height of the triangle)
-- Returns: the area
procedure TArea (float Base, Height) returns (float TA)
  signals (Err)
  purpose ("Calculates the area of a right triangle") is

 begin
  if Base <= 0 | Height <= 0 then   -- check for valid input
    signal Err                      -- signal ERR if not valid
  else
    TA = Base/2 * Height            -- else return area
  end if
 end
  except on others (string SignalName) do
    Print("Signal ", SignalName, " raised.")
    end except

 end procedure TArea

-- CVolume calculates the volume of a cylinder.
-- Inputs: 2 floats (the radius of the cylinder's circular part,
--                    and the height of the cylinder)
-- Returns: a float (the volume)
procedure CVolume (float Radius, Height) returns (float CV)
  signals (Err)
  purpose ("Calculates the volume of a cylinder") is

 begin
  if Radius <= 0 | Height <= 0 then   -- check for valid input
    signal Err                        -- signal ERR if not valid
  else
    CV = Pi() * Radius^2 * Height
  end if
 end
  except on others (string SignalName) do
    Print("Signal ", SignalName, " raised.")
    end except

 end procedure CVolume
