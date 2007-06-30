//
// Author: Mihail Zenkov <mihail.zenkov@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"

#if defined(USE_JACK) && !defined(ADM_WIN32)

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "default.h"
#include "ADM_audiodevice.h"
#include <ADM_assert.h>
#include "ADM_audiodevice/ADM_deviceoss.h"
#include "ADM_audiodevice/ADM_deviceALSA.h"
#include "ADM_audiodevice/ADM_deviceJack.h"

jackAudioDevice::jackAudioDevice()
{
	client = NULL;
}

void jackAudioDevice::jack_shutdown(void *arg)
{
	((jackAudioDevice*)arg)->stop();
}

uint8_t jackAudioDevice::stop()
{
	if (client) {
		jack_client_close(client);
		client = NULL;
		jack_ringbuffer_free(ringbuffer);
	}

	return 1;
}

uint8_t jackAudioDevice::init(uint8_t channels, uint32_t fq)
{
	jack_status_t status;
	_channels = channels;

	if (sizeof(jack_default_audio_sample_t) != sizeof(float)) {
		printf("[JACK] jack_default_audio_sample_t != float\n");
		return 0;
	}

	client = jack_client_open("avidemux", JackNullOption, &status, NULL);

	if (client == NULL) {
		printf("[JACK] jack_client_open() failed, status = 0x%2.0x\n", status);
		if (status & JackServerFailed)
			printf(("[JACK] Unable to connect to server\n"));
		return 0;
	}

	if (status & JackServerStarted)
		printf("[JACK] Server started\n");

	jack_set_process_callback(client, process_callback, this);
	jack_on_shutdown(client, jack_shutdown, this);

	ringbuffer = jack_ringbuffer_create(65536 * channels);

	char name[10];
	for (int i = 0; i < channels; i++) {
		snprintf(name, 10, "output-%d", i);
		ports[i] = jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
		if (!ports[i]) {
			printf("[JACK] Can't create new port\n");
			stop();
			return 0; 
		}
	}

	if (jack_activate(client)) {
		printf("[JACK] Cannot activate client\n");
		stop();
		return 0;
	}

	const char **input_ports = jack_get_ports(client, NULL, NULL, JackPortIsInput|JackPortIsPhysical);

	for (int i = 0; i < channels && input_ports[i]; i++) {
		if (jack_connect(client, jack_port_name(ports[i]), input_ports[i]))
			printf("[JACK] Connecting failed\n");
	}

	if (channels == 1 && input_ports[1])
		if (jack_connect(client, jack_port_name(ports[0]), input_ports[1]))
			printf("[JACK] Connecting failed\n");

	return 1;
}

int jackAudioDevice::process(jack_nframes_t nframes)
{
	jack_default_audio_sample_t *pbuf[_channels];
	for (int c = 0; c < _channels; c++)
		pbuf[c] = (jack_default_audio_sample_t *) jack_port_get_buffer(ports[c], nframes);

	size_t read = jack_ringbuffer_read_space(ringbuffer) / sizeof(jack_default_audio_sample_t) / _channels;

	if (read > nframes)
		read = nframes;

	int i;
	for (i = 0; i < read; i++)
		for (int c = 0; c < _channels; c++) {
			jack_ringbuffer_read(ringbuffer, (char *)pbuf[c], sizeof(jack_default_audio_sample_t));
			pbuf[c]++;
		}

	for (; i < nframes; i++)
		for (int c = 0; c < _channels; c++) {
			pbuf[c] = 0;
			pbuf[c]++;
		}

	if (read != nframes)
		printf("[JACK] UNDERRUN!\n");



	return 0;
}

int jackAudioDevice::process_callback(jack_nframes_t nframes, void* arg)
{
	return ((jackAudioDevice*)arg)->process(nframes);
}


uint8_t jackAudioDevice::play(uint32_t len, float *data)
{
	size_t write;

	while (len) {
		if ((write = jack_ringbuffer_write_space(ringbuffer) / sizeof(jack_default_audio_sample_t)) > _channels) {
			if (write > len)
				write = len;
			jack_ringbuffer_write(ringbuffer, (char *)data, write * sizeof(jack_default_audio_sample_t));
			data += write;
			len -= write;
		} else {
			printf("[JACK] OVERRUN!\n");
			usleep(10000);
		}
	}

        return 1;
}

uint8_t jackAudioDevice::setVolume(int volume){
#ifdef OSS_SUPPORT
	ossAudioDevice dev;
	dev.setVolume(volume);
#else
#ifdef ALSA_SUPPORT
	alsaAudioDevice dev;
	dev.setVolume(volume);
#endif
#endif
	return 1;
}

#else
void dummy_jack_func( void);
void dummy_jack_func( void)
{
}
#endif
