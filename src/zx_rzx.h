// rzx ------------------------------------------------------------------------

RZX_EMULINFO rzx_info;
rzx_u16 rzx_icount;
rzx_u32 rzx_tstates=555;
uint64_t rzx_counter;
int rzx_last_port;
int rzx_frame;

int loadfile(const char *file, int preloader);

rzx_u32 RZX_callback(int msg, void *blob) {
    RZX_IRBINFO *irb = (RZX_IRBINFO*)blob;
    RZX_SNAPINFO *snap = (RZX_SNAPINFO*)blob;

    /**/ if( msg == RZXMSG_CREATOR ) {

    }
    else if( msg == RZXMSG_LOADSNAP ) {
        printf("> LOADSNAP: '%s' (%i bytes) %s %s\n",
            snap->filename,
            (int)snap->length,
            snap->options & RZX_EXTERNAL ? "#external" : "#embedded",
            snap->options & RZX_COMPRESSED ? "#compressed" : "#uncompressed");
        loadfile(snap->filename, 0);
    }
    else if( msg == RZXMSG_IRBNOTIFY ) {
        rzx_counter = cpu.fetches;

        int frm = irb->framecount;
        int tst = irb->tstates;
        int opt = irb->options;

        if( rzx.mode == RZX_PLAYBACK ) {
            // fetch the IRB info if needed
            rzx_tstates = irb->tstates;
            printf("> IRB notify: frames=%i tstates=%i %s\n", (int)frm, (int)rzx_tstates,
                irb->options & RZX_COMPRESSED ? "#compressed" : "#uncompressed");
        }
        else if( rzx.mode == RZX_RECORD ) {
            // fill in the relevant info, i.e. tstates, options
            irb->tstates = rzx_tstates;
            irb->options = 0;
            #ifdef RZX_USE_COMPRESSION
            irb->options |= RZX_COMPRESSED;
            #endif
            printf("> IRB notify: tstates=%i %s\n", (int)rzx_tstates,
                irb->options & RZX_COMPRESSED ? "#compressed" : "#uncompressed");
        }
    } else {
        printf("> MSG #%02X\n", msg);
        return RZX_INVALID;
    }

    return RZX_OK;
}

bool RZX_stop() {
    // if recording or playback,
    // Closes the RZX file. Must be called when the recording is finished. It is called automatically at the end of playback.
    if( rzx.mode ) {
        rzx_close();
    }
    rzx_frame = 0;
    rzx_counter = 0;
    rzx_last_port = 0;
    return 1;
}
void RZX_reset() {
    RZX_stop();
}
bool RZX_play(const char *filename, int rec) {
    memset(&rzx_info, 0, sizeof(RZX_EMULINFO));
    strcpy(rzx_info.name, "Spectral");
    rzx_info.ver_major = atoi(SPECTRAL+1); // "v1.02"
    rzx_info.ver_minor = atoi(SPECTRAL+3);
    if( rzx_init(&rzx_info, RZX_callback) != RZX_OK )
        return 0;

    RZX_reset();

    if( rec ) {
        if( rzx_record(filename) != RZX_OK )
            return alert("Unable to start recording"), 0;
        // Store a snapshot into the RZX.
        // @todo: z80_save("spectral.$$");
        if( rzx_add_snapshot("spectral.$$", RZX_COMPRESSED) != RZX_OK )
            return alert("Unable to insert snapshot"), 0;
        unlink("spectral.$$");
        return 1;
    }
    else {
        if( rzx_playback(filename) != RZX_OK )
            return alert("Unable to start playback"), 0;
        return 1;
    }

    return 0;
}
bool RZX_load(const void *ptr, int len) {
    if( memcmp(ptr, "RZX!", 4) ) return 0;

    bool ok = 0;
    for( FILE *fp = fopen(".Spectral.rzx", "wb"); fp; fclose(fp), fp = 0) {
        ok = fwrite(ptr, len, 1, fp);
    }
    ok = ok && RZX_play(".Spectral.rzx", 0);
    return unlink(".Spectral.rzx"), ok;
}

void RZX_tick() {

    if( rzx.mode == RZX_IDLE ) {
        return;
    }

    if( rzx.mode == RZX_PLAYBACK ) {
        rzx_icount = rzx_counter - cpu.fetches;
        rzx_counter = cpu.fetches;
    }

    if( rzx.mode == RZX_RECORD ) {
        printf("frame %04i: rzx_icount=%05i(%04X)\n", rzx_frame, rzx_icount, rzx_icount);

        // In recording mode, it writes the current frame (which is icount instructions long)
        // to the RZX file and it is called when an interrupt occurs (or it would occur, if
        // maskable interrupts are disabled).
        rzx_update(&rzx_icount);
    }
    if( rzx.mode == RZX_PLAYBACK ) {
        // In playback mode, this function is called to start a new frame (the new icount is supplied)
        int err = rzx_update(&rzx_icount);
        if( err != RZX_OK ) {
            const char *errors[] = {
            "RZX_OK", // 0
            "RZX_NOTFOUND", // -1
            "RZX_INVALID", // -2
            "RZX_FINISHED", // -3
            "RZX_UNSUPPORTED", // -4
            "RZX_NOMEMORY", // -5
            "RZX_SYNCLOST", // -6
            };
            alert(va("rzx playback err: %d", err, errors[-err]));
            return;
        }

        printf("frame %04i: rzx_icount=%05i(%04X)\n", rzx_frame, rzx_icount, rzx_icount);
    }

    rzx_frame++;
}
