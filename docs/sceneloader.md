# Parser Documentation

## General Information
This parser processes lines of configuration data from a file, with each line representing a distinct type of object or setting. The lines must be enclosed in angle brackets (`<` and `>`). Only lines beginning with an angle bracket are considered for processing. The line is then parsed into a key-value format, where the key represents the object or setting type and the values represent specific attributes or parameters for that type.

## Format of Each Line
- Each line should be in the format: `<type:parameter1=value1;parameter2=value2;...>`
- Parameters are separated by semicolons (`;`).
- Each parameter includes a key and value, separated by an equals sign (`=`).

## Supported Types and Parameters
1. **object/player**
   - `meshname`: Path to the mesh file (String)
   - `position`: Object position as vector (e.g., `(x,y,z)`, optional)
   - `scale`: Object scaling factors (e.g., `(x,y,z)`, optional)
   - `rotation`: Object rotation angles (e.g., `(x,y,z)`, optional)
   - `physics`: Enables physics if set to `true` (Boolean, optional)
   - `fixed`: Object immobility if set to `true` (Boolean, optional)
   - `program`: Shader program to be used (String, "light" or "")

2. **background**
   - `face1` to `face6`: Paths to the textures for each face of a cubemap (String)

3. **light**
   - not yet supported

4. **camera**
   - `position`: Camera position (e.g., `(x,y,z)`, optional)
   - `fov`: Field of view in degrees (Integer, optional)
   - `up`: Upward direction vector (e.g., `(x,y,z)`, optional)
   - `yaw`: Horizontal angle (Float, optional)
   - `pitch`: Vertical angle (Float, optional)

## Exception Handling
The parser will skip lines that do not meet the format requirements or have missing values. It throws an exception if necessary components like `meshpath` for objects or cube faces for backgrounds are missing.

## Special Considerations
- Values enclosed in double quotes are treated as strings.
- Numeric values followed by `f` are treated as floats.
- Tuple values are parsed into vectors (e.g., `(x,y,z)`).
