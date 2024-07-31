#include <stdio.h>
#include <stdlib.h>
#include <pulse/pulseaudio.h>

static pa_sink_info *currentSink = NULL;
static pa_operation *currentOperation = NULL;

void sinkInfoCallback(pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
    if (!eol) currentSink = (pa_sink_info *)info;
}

void volumeCallback(pa_context *context, int success, void *userdata) {
    if (!success) fprintf(stderr, "Volume change failed *><\n");
    pa_operation_unref(currentOperation);
    currentOperation = NULL;
}

void setVolume(pa_context *context, int delta) {
    pa_cvolume volume;
    unsigned int maxVolume = PA_VOLUME_NORM;

    if (!currentSink) {
        fprintf(stderr, "No sink info available T-T\n");
        return;
    }

    volume = currentSink->volume;
    unsigned int changeAmount = maxVolume * abs(delta) / 100;

    if (delta > 0) pa_cvolume_inc(&volume, changeAmount);
    else pa_cvolume_dec(&volume, changeAmount);

    currentOperation = pa_context_set_sink_volume_by_index(context, currentSink->index, &volume, volumeCallback, NULL);
    if (!currentOperation) fprintf(stderr, "Failed to set volume >_<\n");
}

int main(int argc, char *argv[]) {
    pa_mainloop *mainLoop;
    pa_mainloop_api *api;
    pa_context *context;
    int result = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <volume change> :3\n", argv[0]);
        return 1;
    }

    int delta = atoi(argv[1]);
    if (delta < -100 || delta > 100) {
        fprintf(stderr, "Volume change must be between -100 and 100! OwO\n");
        return 1;
    }

    mainLoop = pa_mainloop_new();
    if (!mainLoop) {
        fprintf(stderr, "Failed to create mainloop o_O\n");
        return 1;
    }

    api = pa_mainloop_get_api(mainLoop);
    context = pa_context_new(api, "CMixer");
    if (!context) {
        fprintf(stderr, "Failed to create context X-X\n");
        pa_mainloop_free(mainLoop);
        return 1;
    }

    pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);

    while (pa_context_get_state(context) != PA_CONTEXT_READY) {
        pa_mainloop_iterate(mainLoop, 1, NULL);
    }

    pa_context_get_sink_info_list(context, sinkInfoCallback, NULL);
    while (!currentSink) pa_mainloop_iterate(mainLoop, 1, NULL);

    setVolume(context, delta);

    while (currentOperation) pa_mainloop_iterate(mainLoop, 1, NULL);

    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_mainloop_free(mainLoop);

    return result;
}
