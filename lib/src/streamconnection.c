// SPDX-License-Identifier: LicenseRef-AGPL-3.0-only-OpenSSL

#include <chiaki-cli.h>
#include <chiaki/session.h>
#include <chiaki/base64.h>
#include <argp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep((x)*1000)
#else
#include <unistd.h>
#endif

static char doc[] = "Connect to PS4/PS5 without audio or video (control only).";

#define ARG_KEY_HOST       'h'
#define ARG_KEY_ACCOUNT_ID 'a'
#define ARG_KEY_PS5        '5'

static struct argp_option options[] = {
	{ "host",       ARG_KEY_HOST,       "Host",       0, "PS4/PS5 host address", 0 },
	{ "account-id", ARG_KEY_ACCOUNT_ID, "AccountID",  0, "PSN Account ID (base64)", 0 },
	{ "ps5",        ARG_KEY_PS5,        NULL,         0, "Connect to PS5 (default: PS4)", 0 },
	{ 0 }
};

typedef struct arguments
{
	const char *host;
	const char *account_id;
	bool ps5;
} Arguments;

static int parse_opt(int key, char *arg, struct argp_state *state)
{
	Arguments *arguments = state->input;
	switch(key)
	{
		case ARG_KEY_HOST:
			arguments->host = arg;
			break;
		case ARG_KEY_ACCOUNT_ID:
			arguments->account_id = arg;
			break;
		case ARG_KEY_PS5:
			arguments->ps5 = true;
			break;
		case ARGP_KEY_ARG:
			argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, 0, doc, 0, 0, 0 };

static void session_cb(ChiakiEvent *event, void *user)
{
	ChiakiLog *log = user;
	switch(event->type)
	{
		case CHIAKI_EVENT_CONNECTED:
			CHIAKI_LOGI(log, "Connected to PS4/PS5!");
			break;
		case CHIAKI_EVENT_LOGIN_PIN_REQUEST:
			CHIAKI_LOGI(log, "Login PIN requested (not supported in CLI mode)");
			break;
		case CHIAKI_EVENT_QUIT:
			CHIAKI_LOGI(log, "Session quit: reason=%d", event->quit.reason);
			break;
		default:
			break;
	}
}

static void audio_cb(int16_t *buf, size_t samples_count, void *user)
{
	// Discard audio - we don't want it
	(void)buf;
	(void)samples_count;
	(void)user;
}

static void video_cb(uint8_t *buf, size_t buf_size, void *user)
{
	// Discard video - we don't want it
	(void)buf;
	(void)buf_size;
	(void)user;
}

CHIAKI_EXPORT int chiaki_cli_cmd_connect(ChiakiLog *log, int argc, char *argv[])
{
	Arguments arguments = { 0 };
	error_t argp_r = argp_parse(&argp, argc, argv, ARGP_IN_ORDER, NULL, &arguments);
	if(argp_r != 0)
		return 1;

	if(!arguments.host)
	{
		fprintf(stderr, "No host specified, see --help.\n");
		return 1;
	}
	if(!arguments.account_id)
	{
		fprintf(stderr, "No account-id specified, see --help.\n");
		return 1;
	}

	// Decode base64 account id
	uint8_t account_id[8];
	size_t account_id_size = sizeof(account_id);
	ChiakiErrorCode err = chiaki_base64_decode(arguments.account_id, strlen(arguments.account_id), account_id, &account_id_size);
	if(err != CHIAKI_ERR_SUCCESS || account_id_size != 8)
	{
		fprintf(stderr, "Invalid account-id (must be base64 encoded 8 bytes)\n");
		return 1;
	}

	ChiakiConnectInfo info;
	memset(&info, 0, sizeof(info));
	info.host = arguments.host;
	info.ps5 = arguments.ps5;
	memcpy(info.regist_key, account_id, sizeof(account_id));

	// No audio, no video
	info.video_profile.width = 0;
	info.video_profile.height = 0;
	info.audio_header.channels = 0;

	ChiakiSession session;
	err = chiaki_session_init(&session, &info, log);
	if(err != CHIAKI_ERR_SUCCESS)
	{
		CHIAKI_LOGE(log, "Session init failed: %s", chiaki_error_string(err));
		return 1;
	}

	chiaki_session_set_event_cb(&session, session_cb, log);
	chiaki_session_set_video_sample_cb(&session, video_cb, NULL);
	chiaki_session_set_audio_sink(&session, audio_cb, NULL);

	err = chiaki_session_start(&session);
	if(err != CHIAKI_ERR_SUCCESS)
	{
		CHIAKI_LOGE(log, "Session start failed: %s", chiaki_error_string(err));
		chiaki_session_fini(&session);
		return 1;
	}

	CHIAKI_LOGI(log, "Connecting to %s... Press Ctrl+C to quit.", arguments.host);

	err = chiaki_session_join(&session);
	chiaki_session_fini(&session);

	return err == CHIAKI_ERR_SUCCESS ? 0 : 1;
}
