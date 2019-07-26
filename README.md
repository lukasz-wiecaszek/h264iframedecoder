# h264iframedecoder

Back in the 90-ties ...,
ok, maybe not so long ago, I wrote an application which was able to transcode
MPEG2 (ISO/IEC 13818-2) video I-frames directly to JPEG files.
This transcoding was based on copying macroblocks from video I-frames to JPEG macroblocks.
So in fact no real decoding took place, which was exactly what I wanted to achive.
Then I thought maybe the same trick will be possible for H264 video (ISO/IEC 14496-10).
So I started such a project, but after deeper studying and understanding the specification
it appeared, that this is not possible.
Nevertheless, I decided that I will finish that project but I will try to decode H264 I-framses.
And this is it. Still under development, project called h264iframedecoder.

