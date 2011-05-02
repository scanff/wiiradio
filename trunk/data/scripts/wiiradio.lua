-- DEFAULTS FOR WIIRADIO

-- Screen defines
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 480

SCREEN_MAIN = 0
SCREEN_BROWSER =  1
SCREEN_PLAYLISTS = 2
SCREEN_GENRES = 3
SCREEN_CANCEL_CON_BUF = 4
SCREEN_SEARCHING = 5
SCREEN_LOCALFILES = 6
SCREEN_STREAM_INFO = 21
SCREEN_OPTIONS = 22
SCREEN_SEARCHGENRE = 23
SCREEN_CONNECT = 24

--// SYSTEM VARS
SYS_RIPPING = 1
SYS_MUTE = 1
SYS_VOLUME= 3

SYS_STOPPED = 1
SYS_PLAYING = 2
SYS_PAUSE = 3
SYS_BUFFERING = 4
SYS_FAILED = 5
SYS_CONNECTING = 6
SYS_SEEKING = 7
SYS_LOCAL_BUFFERING = 12

-- windows that don't effect state
WND_SEARCHING = 8
WND_SELECTMEDIA = 9
WND_PRESEARCH = 100

WND_STATUS = 0

-- status 
SYS_STATUS = SYS_STOPPED
SYS_SCREEN = SCREEN_MAIN
SYS_LISTVIEW = SCREEN_PLAYLISTS

RIP_STATUS = "... Ripping Status"
KB_OUTPUT = ""


ws = 0
st = 0
ss = 0
skin = ""
lang = ""
service = 0
ripmusic = 0
startfromlast = 0
volume = 0
visualmode = 0
sleepafter = 0
sortingfolders = 0

-- variables

BITRATE = "N/A"
ARTIST = "N/A"
TITLE = "N/A"
ALBUM = "N/A"
GENRE = "N/A"
YEAR = "N/A"

NOW_PLAYING = ""
TRACK_TITLE = ""
SONG_POS ="00:00"
SONG_POS_SEC = 0


-- stores the script data defined in the skin file

skin_script = ""


lastkey = ""
lastscreen = 0
--
xi=0
xdir=5
function direct_connect()
SYS_SCREEN = SCREEN_MAIN
end

function main()

	-- and any main loop stuff in here

	if SYS_STATUS == SYS_STOPPED then
		NOW_PLAYING = "Stopped"
	elseif SYS_STATUS == SYS_FAILED then
		NOW_PLAYING = "Failed"
	elseif SYS_STATUS == SYS_BUFFERING then
		NOW_PLAYING = "Buffering"
	elseif SYS_STATUS == SYS_PLAYING then
		NOW_PLAYING = "Playing : " .. TRACK_TITLE
	elseif SYS_STATUS == SYS_PAUSE then
		NOW_PLAYING = "Paused"
	elseif SYS_STATUS == SYS_CONNECTING then
		NOW_PLAYING = "Connecting"
	else
		NOW_PLAYING = "Error"
	end

--  An example of interacting w/ a gui skin object	
--	if xi > 100 then
--	   xdir = -5
--	elseif xi < -100 then
--	   xdir = 5
--	end
	
--	xi = xi + xdir
--	p = wrSetObj()
--	p('myvisual','x',xi)
	
end

function onkey(ikey)

	if ikey == "1" then
		if SYS_SCREEN == SCREEN_STREAM_INFO then
			SYS_SCREEN = SCREEN_MAIN
		elseif SYS_SCREEN == SCREEN_MAIN then
			SYS_SCREEN = SCREEN_STREAM_INFO
		end
	end

end
