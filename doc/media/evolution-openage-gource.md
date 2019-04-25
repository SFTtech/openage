# Reproducing the gource 'evolution video'

You can find the video here:
https://www.youtube-nocookie.com/embed/CrccACCWQG0

Download, compile & install git, gource, ffmpeg + FLOSS video cutting tool.

1. Set git directory to 'Master' branch
2. Open terminal/command line and create a new folder for the video
3. Copy the logo in \*.png-format into the new folder (svg->png with Inkscape)
4. Navigate to the new folder
5. Use the following command

```gource --camera-mode overview --title https://openage.sft.mx --seconds-per-day 0.05 --auto-skip-seconds 0.1 --max-file-lag 0.1 --file-idle-time 0 --background 555544 --logo <logo.png> --font-size 15 --date-format "%B %Y" --hide filenames, mouse -1920x1080 -o <outputfile-name.ppm> <../path to git directory>```

6. Use ffmpeg to convert the ppm file into a video container with the following command

```<path-to-ffmpeg(.exe)> -y -r 60 -f image2pipe -vcodec ppm -i <gource-outputfile-name.ppm> -vcodec libx264 -preset fast -pix_fmt yuv420p -crf 1 -threads 4 -bf 0 <output-filename>.x264.avi```

7. Use video cutting tool and music licensed under creative commons
8. Render!
