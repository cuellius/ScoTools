# ScoTools
Tools for externally editing Mount&amp;Blade scenes with image editing programs.

The kit includes four programs:
<pre>
1) ScoPng [sceneobj file] [optional png output filename]
   Saves the heightmap in PNG, TGA or BMP. Just change the extension
2) PngSco [sceneobj file] [png input file] [optional sceneobj output]
   Supports PNG, PSD, TGA, GIF, JPG heightmap files as input
3) PsdToPng [input image] [optional png output filename]
   Converts PSD, TGA, GIF, JPG, BMP to PNG. Just drop the file on the program's icon
4) ScoToObj [sceneobj file] [obj output filename]
   .Sco to .Obj converter
</pre>

Based on Swyter's works (https://bitbucket.org/Swyter/scotools/src/default/) and on Barabas's works (https://forums.taleworlds.com/index.php/topic,221975.msg5536918.html#msg5536918)
Uses stb_image/_write, by Sean Barrett and contributors. Under the public domain.
