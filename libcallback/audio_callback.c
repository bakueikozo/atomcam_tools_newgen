#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <tinyalsa/pcm.h>

struct frames_st {
  void *buf;
  size_t length;
};
typedef int (* framecb)(struct frames_st *);

static uint32_t (*real_local_sdk_audio_set_pcm_frame_callback)(int ch, void *callback);
static void *audio_pcm_cb = NULL;
static int AudioCaptureEnable = 0;
static struct pcm *pcm = NULL;

char *AudioCapture(int fd, char *tokenPtr) {

  char *p = strtok_r(NULL, " \t\r\n", &tokenPtr);
  if(!p) {
    if(!pcm) return "disabled";
    return AudioCaptureEnable ? "on" : "off";
  }
  if(!strcmp(p, "on")) {
    AudioCaptureEnable = 1;
    printf("[command] audio capute on\n", p);
    return "ok";
  }
  if(!strcmp(p, "off")) {
    AudioCaptureEnable = 0;
    printf("[command] audio capute off\n", p);
    return "ok";
  }
  return "error";
}

static uint32_t audio_pcm_capture(struct frames_st *frames) {

  static int firstEntry = 0;
  uint32_t *buf = frames->buf;

  if(!pcm) {
    const struct pcm_config config = {
      .channels = 1,
      .rate = 8000,
      .format = PCM_FORMAT_S16_LE,
      .period_size = 320,
      .period_count = 8,
      .start_threshold = 320 * 4,
      .silence_threshold = 0,
      .silence_size = 0,
      .stop_threshold = 320 * 8,
      .avail_min = 0,
    };
    pcm = pcm_open(0, 1, PCM_OUT | PCM_MMAP, &config);
    if(pcm == NULL) {
        fprintf(stderr, "failed to allocate memory for PCM\n");
    } else if(!pcm_is_ready(pcm)) {
      fprintf(stderr, "failed to open PCM : %s\n", pcm_get_error(pcm));
      pcm_close(pcm);
      pcm = NULL;
    }
  }

  if(pcm && AudioCaptureEnable) {
    int err = pcm_writei(pcm, buf, pcm_bytes_to_frames(pcm, frames->length));
    if(err < 0) fprintf(stderr, "pcm_writei err=%d\n", err);
  }
  return ((framecb)audio_pcm_cb)(frames);
}

uint32_t local_sdk_audio_set_pcm_frame_callback(int ch, void *callback) {

  fprintf(stderr, "local_sdk_audio_set_pcm_frame_callback streamChId=%d, callback=0x%x\n", ch, callback);
  if(ch == 0) {
    audio_pcm_cb = callback;
    fprintf(stderr,"enc func injection save audio_pcm_cb=0x%x\n", audio_pcm_cb);
    callback = audio_pcm_capture;
  }
  return real_local_sdk_audio_set_pcm_frame_callback(ch, callback);
}

static void __attribute ((constructor)) audio_callback_init(void) {

  real_local_sdk_audio_set_pcm_frame_callback = dlsym(dlopen("/system/lib/liblocalsdk.so", RTLD_LAZY), "local_sdk_audio_set_pcm_frame_callback");
}
