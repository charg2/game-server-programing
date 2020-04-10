/*
Copyright 2017 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/


// Release 빌드 오류 https://m.blog.naver.com/PostView.nhn?blogId=jungwan82&logNo=220628992268&proxyReferer=https%3A%2F%2Fwww.google.com%2F 이거 보고 해결
//

#include "stdafx.h"
#include "KeyManager.h"
#include "EchoClient.h"
#include "Framework.h"
#include "OtherPlayer.h"
#include "../../Common/dump.h"



int main(int argc, char **argv)
{
	SetUnhandledExceptionFilter(ExceptionFilter);
	g_key_input.reset(new KeyInput);
	g_echo_client.reset(new EchoClient);


	Framework game{};

	if (false == game.initialize(argc, argv))
		return  -1;

	OtherPlayer::init();

	g_echo_client->initialize();

	g_echo_client->connect_using_input();

	game.run();

	g_echo_client->finalize();

	OtherPlayer::fin();

	game.finalize();

    return 0;
}



