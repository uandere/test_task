# Test task (mp4 to mkv (Opus & av1) using GStreamer)

## Description

This project aims to process media in the following manner:

- the program accepts a video file 
- demuxes it, making separate audio and video streams
- converts audio stream to Opus codec
- converts video stream to av1 codec
- muxes new audio and video streams together
- finally, it saves new video inside MKV container

The pipeline of the program can be represented as:

![image](https://github.com/uandere/test_task/assets/136515158/c76dcd2b-2bd3-4b48-93ce-0f039c7b43d6)

## Prerequisites

You need to make sure these tools are installed on your system:

- GStreamer (standard installation, follow this [link](https://gstreamer.freedesktop.org/documentation/installing/on-linux.html?gi-language=c))
- Cmake (3.15+)
- g++/clang/msvc (with C++17 support)

## Usage

To use this tool, let's first clone this repo:

```shell
git clone https://github.com/uandere/test_task.git
cd test_task
```

Next, let's compile the program. To do this, you shall use the `compile.sh` bash script:

```shell
./compile.sh -O -d
```

We're passing `-O` option to indicate that we want to compile it with **maximum** compiler optimizations,
and `-d` to tell the compiler **ignore** all debug information. This will produce the most performant version
of the tool. This command will also **install** our binary, which you can then find in `/bin` subdirectory.

Finally, to use this tool, we can use the helper information:

```shell
./bin/test_task -h
```

This will tell us, how are we supposed to use the program:

`Usage: ./bin/test_task <input_file> <output_file> --verbose[optional]
`

Good! Let's finally try to process some video:

```shell
./bin/test_task samples/input.mp4 samples/output.mkv
```

Here, we're passing samples/input.mp4 file (it is included inside the repo) to 
the program. The resulting file will have location `samples/output.mkv`.

We can also use the **verbose** version of the tool

```shell
./bin/test_task samples/input.mp4 samples/output.mkv --verbose
```

This will do the same job, but will additionally provide us with useful
progress information to track program's state:

```
muxed_completed: (00:00:39): 0 / 5 seconds ( 0.0 %)
audio_processed: (00:00:39): 0 / 5 seconds ( 0.0 %)
video_processed: (00:00:39): 0 / 5 seconds ( 0.0 %)
```

Here you can see the percentage of the job done and overall time spent.

## Code review

I used GStreamer library alongside with CMake to get this done. Let's see how it works.

After parsing user parameters, we're creating a string representation of our pipeline:

```c++
// Pipeline description string
std::string pipeline_description_str = std::string("filesrc location=") + inputFile +
                                       " ! qtdemux name=demux demux.audio_0 ! queue ! decodebin " + audio_progress +
                                       " ! audioconvert ! audioresample ! opusenc "
                                       " ! mux.  demux.video_0 ! queue ! decodebin " + video_progress +
                                       " ! videoconvert ! av1enc ! mux. matroskamux name=mux " + mux_progress +
                                       " ! filesink location=" + outputFile;
```

This includes all the options and parameters that user passed.

Next, we're creating a pipeline using `gst_parse_launch()` function that GStreamer
provides:

```c++
GstElement *pipeline = gst_parse_launch(pipeline_description, nullptr);
```

It is very useful in this case, as creating all the items manually, without
parsing, would be super boilerplate.

After that, we're just starting main loop and waiting for the `ERROR` or `EOS`
message. We handle them properly, gracefully clearing the pipeline after finishing
and returning the corresponding error code (or 0 in success case).

Argument parsing is implemented manually (because of the simplicity of the project)
inside the `utils.h/cpp` files. 

## Results

After executing a program on a input.mp4 (https://download.samplelib.com/mp4/sample-5s.mp4),
we can see the output file in .mkv format, and can easily verify the codecs used. 
In this case, the input video was ~2.8 MB, the output is ~118 MB. On my machine, the
overall time taken for processing is ~36 mins, but on your machine it can be much
faster, depending on how good your hardware is for av1 codec.

## Conclusions

It was cool to play around using GStreamer, discovering its philosophy and primary 
tools. I experimented with many codecs and containers, comparing performance and 
compability for each of them.
