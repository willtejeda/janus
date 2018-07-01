#include "audioutil.h"

const int AudioUtil::_FRAME_SIZE = 2880;
const int AudioUtil::_MAX_FRAME_SIZE = 2880 * 6;
const int AudioUtil::_SAMPLE_RATE = 48000;
const int AudioUtil::_CHANNELS = 1;
const int AudioUtil::_APPLICATION = OPUS_APPLICATION_VOIP;
const int AudioUtil::_MAX_PACKET_SIZE = 2880*8;

OpusEncoder * AudioUtil::encoder = NULL;
OpusDecoder * AudioUtil::decoder = NULL;

AudioUtil::AudioUtil()
{
}

AudioUtil::~AudioUtil()
{
}

QByteArray AudioUtil::encode(QByteArray decoded)
{
    //Create Opus encoder state
    int error = 0;
    if (encoder == NULL) {
        encoder = opus_encoder_create(AudioUtil::_SAMPLE_RATE, AudioUtil::_CHANNELS, AudioUtil::_APPLICATION, &error);
        opus_encoder_ctl(encoder, OPUS_SET_MAX_BANDWIDTH(OPUS_BANDWIDTH_MEDIUMBAND));
        opus_encoder_ctl(encoder, OPUS_SET_BITRATE(48000)); //4KB/sec
    }
    if (error<0)
    {
        // Failed to create encoder state
        qDebug("ERROR: Failed to create encoder state");        
        return QByteArray();
    }   

    //Encode a frame
    const unsigned int s = AudioUtil::_FRAME_SIZE*AudioUtil::_CHANNELS;
    opus_int16 in[s];
    memcpy(in, decoded.data(), s*sizeof(opus_int16)); //conversion from samples to 16-bit type

	QByteArray encoded;
	encoded.resize(AudioUtil::_MAX_PACKET_SIZE);

	unsigned char * encoded_str = (unsigned char *)encoded.data();
    const int len = opus_encode(encoder, in, AudioUtil::_FRAME_SIZE, encoded_str, AudioUtil::_MAX_PACKET_SIZE);
//    qDebug() << "encode len" << decoded.length() << len;
    if (len < 0) {
        //qDebug("ERROR: Failed to encode: %s\n", opus_strerror(len));
        return QByteArray();
    }

	encoded.resize(len);
	encoded.squeeze();

    //Return encoded data
    return encoded;
}

QByteArray AudioUtil::decode(QByteArray encoded)
{
    //Create Opus decoder state
    int error = 0;
    if (decoder == NULL) {
        decoder = opus_decoder_create(AudioUtil::_SAMPLE_RATE, AudioUtil::_CHANNELS, &error);
    }
    if (error<0)
    {
        // Failed to create decoder state
        qDebug("ERROR: Failed to create decoder state");
        return QByteArray();
    }

    //Decode a frame
	QByteArray decoded;
	decoded.resize(AudioUtil::_MAX_FRAME_SIZE * AudioUtil::_CHANNELS * sizeof(opus_int16));
	if (decoded.size() != AudioUtil::_MAX_FRAME_SIZE * AudioUtil::_CHANNELS * sizeof(opus_int16))
	{
		qDebug("ERROR: Failed to allocate memory for decoded data");
		return QByteArray();
	}

	unsigned char * encoded_str = (unsigned char *)encoded.data();

	const int len = opus_decode(decoder, encoded_str, encoded.size(), (opus_int16*)decoded.data(), AudioUtil::_MAX_FRAME_SIZE, 0);
    if (len<0)
    {
        //qDebug("ERROR: Failed to decode: %s\n", opus_strerror(len));
        return QByteArray();
    }

    decoded.resize(len * sizeof(opus_int16));
    decoded.squeeze();

    //Return decoded data
    return decoded;
}

/*
QByteArray AudioUtil::decodeOGG(QByteArray encoded){
    QByteArray decoded;

    long cumulative_read = 0;
//    long ogg_total_size = encoded.size();
    int convsize=4096;
    long byteWritten = 0;

    ogg_sync_state   oy; //Sync and verify incoming physical bitstream
    ogg_stream_state os; //Take physical pages, weld into a logical stream of packets
    ogg_page         og; //One Ogg bitstream page. Vorbis packets are inside
    ogg_packet       op; //One raw packet of data for decode
    vorbis_info      vi; //Struct that stores all the static vorbis bitstream settings
    vorbis_comment   vc; //Struct that stores all the bitstream user comments
    vorbis_dsp_state vd; //Central working state for the packet->PCM decoder
    vorbis_block     vb; //Local working space for packet->PCM decode

    char *buffer;
    int  bytes;
    int index = 0; //Where the encoded OGG QByteArray was last read from

    ogg_sync_init(&oy); //Pages can now be read
    while(1)
    {        
        //Repeat if the bitstream is chained
        int eos=0;
        int i;

        //Submit a 4k block to libvorbis' Ogg layer
        buffer = ogg_sync_buffer(&oy, 4096);
        QByteArray encoded_buffer = encoded.mid(index, 4096);
        bytes = 4096;
        if (index + 4096 > encoded.size())
            bytes = encoded.size() % 4096;
        index += encoded_buffer.size();
        for (int j=0;j<encoded_buffer.length();j++) {
            buffer[j]=encoded_buffer[j];
        }
        ogg_sync_wrote(&oy, bytes);
        cumulative_read += bytes;

        //Get the first page
        if(ogg_sync_pageout(&oy,&og)!= 1)
        {
            // Case where data runs out
            if(bytes<4096)
                break;

            qDebug() << "ERROR: Failed to decode OGG, not Vorbis data";
            return decoded; //56.0 - bugfix crash if "encoded" input is empty
        }

        //Get the serial number (and use it to set up a stream) and set up the rest of decode
        ogg_stream_init(&os, ogg_page_serialno(&og));

        //Extract the initial header from the first page and verify that the Ogg bitstream is in fact Vorbis data
        vorbis_info_init(&vi);
        vorbis_comment_init(&vc);

        if(ogg_stream_pagein(&os,&og)<0)
        {
            qDebug() << "ERROR: Failed to decode OGG, stream version mismatch";
        }


        if(ogg_stream_packetout(&os,&op)!=1)
        {
            qDebug() << "ERROR: Failed to decode OGG, no page";
        }

        if(vorbis_synthesis_headerin(&vi,&vc,&op)<0)
        {
            qDebug() << "ERROR: Failed to decode OGG, no vorbis header";
        }

        //Get the next two packets; the comment and codebook headers, which may span multiple pages.
        i=0;
        while(i<2)
        {
            while(i<2)
            {

                int result=ogg_sync_pageout(&oy,&og);
                if(result==0) break; // More data needed
                else if(result==1)
                {
                    ogg_stream_pagein(&os,&og);
                    while(i<2)
                    {                        
                        result=ogg_stream_packetout(&os,&op);
                        if(result==0)break;
                        else if(result<0)
                        {
                            qDebug() << "ERROR: Failed to decode OGG, header data corrupted/missing";
                        }
                        result=vorbis_synthesis_headerin(&vi,&vc,&op);
                        if(result<0)
                        {
                            qDebug() << "ERROR: Failed to decode OGG, secondary header corrupted/missing";
                        }
                        i++;
                    }
                }
            }

            buffer=ogg_sync_buffer(&oy,4096);
            encoded_buffer = encoded.mid(index, 4096);
            bytes = 4096;
            if (index + 4096 > encoded.size()) {
                bytes = encoded.size() % 4096;
            }
            index += encoded_buffer.size();
            for (int j=0;j<encoded_buffer.length();j++) {
                buffer[j]=encoded_buffer[j];
            }
            cumulative_read += bytes;

            if(bytes==0 && i<2)
            {
                qDebug() << "ERROR: Failed to decode OGG, could not find all Vorbis headers";
            }
            ogg_sync_wrote(&oy,bytes);
        }

        //Initialize the Vorbis packet->PCM decoder
        if(vi.channels > 0 && vorbis_synthesis_init(&vd,&vi)==0)
        {
            convsize=4096/vi.channels;

            vorbis_block_init(&vd, &vb); //Local state for most of the decode so multiple block decodes can proceed in parallel

            //Decode loop
            while(!eos)
            {
                while(!eos)
                {
                    int result=ogg_sync_pageout(&oy,&og);
                    if(result==0)
                    {
                        break; //Need more data
                    }
                    else if(result<0)
                    {
                        qDebug() << "ERROR: Failed to decode OGG, data corrupt/missing";
                    }
                    else
                    {
                        ogg_stream_pagein(&os,&og);

                        while(1)
                        {
                            result=ogg_stream_packetout(&os,&op);
                            if(result==0){
                                break; // Need more data
                            }
                            else if (result>0)
                            {
                                //Decode packet
                                float **pcm; // pcm[0] is left; pcm[1] is right
                                int samples; // Size of channel
                                if(vorbis_synthesis(&vb,&op)==0)
                                    vorbis_synthesis_blockin(&vd,&vb);

                                while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
                                {
                                    int j;
//                                    int clipflag=0;
                                    int bout=(samples<convsize?samples:convsize);

                                    //Convert floats to 16 bit signed ints
                                    //qDebug() << vi.channels << vi.rate;
                                    float  *mono=pcm[0];
                                    float  *stereo = nullptr;
									if (vi.channels > 1)
									{
										stereo = pcm[1];
									}

                                    for(j=0;j<bout;j++)
                                    {
                                        int val;
                                        val = floor(mono[j]*32767.f+.5f);

                                        // Handle clipping
                                        if(val>32767)
                                        {
                                            val=32767;
//                                            clipflag=1;
                                        }
                                        if(val<-32768)
                                        {
                                            val=-32768;
//                                            clipflag=1;
                                        }

                                        decoded.push_back(char(val));
                                        decoded.push_back(char(val>>8));

                                        if (vi.channels > 1){
                                            val = floor(stereo[j]*32767.f+.5f);

                                            // Handle clipping
                                            if(val>32767)
                                            {
                                                val=32767;
//                                                clipflag=1;
                                            }
                                            if(val<-32768)
                                            {
                                                val=-32768;
//                                                clipflag=1;
                                            }

                                            decoded.push_back(char(val));
                                            decoded.push_back(char(val>>8));
                                        }
                                    }
                                    byteWritten += 2*bout*vi.channels;
                                    //qDebug() << "Bytes written" << byteWritten;

                                    vorbis_synthesis_read(&vd, bout); // Tell libvorbis how many samples we actually consumed
                                }
                            }
                        }
                        if(ogg_page_eos(&og))
                            eos=1;
                    }
                }

                if(!eos)
                {
                    buffer=ogg_sync_buffer(&oy,4096);

                    encoded_buffer = encoded.mid(index, 4096);
                    bytes = 4096;
                    if (index + 4096 > encoded.size())
                        bytes = encoded.size() % 4096;
                    index += encoded_buffer.size();
                    for (int i=0;i<encoded_buffer.length();i++)
                        buffer[i]=encoded_buffer[i];
                    cumulative_read += bytes;

                    //qDebug() << "Bytes read" << bytes << cumulative_read;
                    ogg_sync_wrote(&oy,bytes);
                    if(bytes==0)
                        eos=1;
                }
            }

            vorbis_block_clear(&vb);
            vorbis_dsp_clear(&vd);

            //Seek back and write the WAV header
            WavHeader wh(vi.rate, 16, vi.channels, byteWritten);
            QByteArray header = wh.getByteArray();
            decoded.push_front(header);
        }
        else
        {
            qDebug() << "ERROR: Failed to decode OGG, corrupt header";
        }

        //Clean up this logical bitstream; before exit check if chained
        ogg_stream_clear(&os);
        vorbis_comment_clear(&vc);
        vorbis_info_clear(&vi);
    }

    //Clean up the framer
    ogg_sync_clear(&oy);

    return decoded;
}

QByteArray AudioUtil::decodeMP3(QByteArray encoded){
    QByteArray decoded;

#ifndef __ANDROID__
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done = 0;
    int err;
    off_t frame_offset;

	int channels = 0;
	int encoding = 0;
    long rate;

    //Initialization
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    //Open the data stream
    bool end_thread = false;
    bool started = false;
    mpg123_open_feed(mh);
    mpg123_feed(mh, (const unsigned char*) encoded.data(), encoded.size());
    do {        
        err = mpg123_decode_frame(mh, &frame_offset, &buffer, &done);
        switch(err) {
            case MPG123_NEW_FORMAT:
                mpg123_getformat(mh, &rate, &channels, &encoding);
                break;
            case MPG123_OK:
                started = true;
                for (unsigned int i = 0; i < done; i++){
                    decoded.push_back(buffer[i]);
                }
                break;
            case MPG123_NEED_MORE:
                end_thread = true;
                break;
            default:
                end_thread = true;
                break;
        }

        //55.9 - on exit, this thread could run indefinitely, leading janusvr not to terminate
        if (end_thread) {
            break;
        }
    } while (!started || done > 0);

    WavHeader wh(rate, 8*mpg123_encsize(encoding), channels, decoded.size());
    QByteArray header = wh.getByteArray();
    decoded.push_front(header);

    //Clean up
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
#endif

    return decoded;
}*/

bool AudioUtil::isWav(QByteArray ba)
{
    if (ba.size() >= 44)
    {
        WavHeader wh(ba.mid(0,44));
        char riff_id[5];
        char riff_format[5];
        char fmt_id[5];
        char data_id[5];
        memcpy(riff_id, wh.riff_id, 4);
        memcpy(riff_format, wh.riff_format, 4);
        memcpy(fmt_id, wh.fmt_id, 4);
        memcpy(data_id, wh.data_id, 4);
        riff_id[4] = '\0';
        riff_format[4] = '\0';
        fmt_id[4] = '\0';
        data_id[4] = '\0';

        if(strcmp(riff_id, "RIFF") == 0 && strcmp(riff_format, "WAVE") == 0 && strcmp(fmt_id, "fmt ") == 0 && strcmp(data_id, "data") == 0) {
            return true;
        }
    }
    return false;
}

QByteArray AudioUtil::stereoToMono(QByteArray stereo)
{
    QByteArray mono;
    if (isWav(stereo)){
        WavHeader wh(stereo.mid(0,44));
        if (wh.fmt_channels == 2){
            //Change header
            wh.riff_size = 44 + wh.data_size/2;
            wh.fmt_channels = 1;
            wh.fmt_block_align = wh.fmt_channels*wh.fmt_bits_per_sample/8;
            wh.fmt_byte_rate = wh.fmt_channels*wh.fmt_bits_per_sample*wh.fmt_sample_rate/8;
            wh.data_size = wh.data_size/2;
            mono.push_back(wh.getByteArray());
            stereo.remove(0,44);

            //Iterate through each pair of samples and average out
            //Weight one channel more to prevent out of phase zeroing out
            if (wh.fmt_bits_per_sample == 16){               
                for (int i = 0; i < (int) wh.data_size; i+=2){
                    short val = 0;
                    memcpy(&val, &stereo.data()[2*i], sizeof(short));
                    mono.push_back(char(val));
                    mono.push_back(char(val>>8));
                }
            }
            else if (wh.fmt_bits_per_sample == 8){               
                for (int i = 0; i < (int) wh.data_size; i++){
                    unsigned char val = 0;
                    memcpy(&val, &stereo.data()[2*i], sizeof(unsigned char));
                    mono.push_back((unsigned char)(val));
                }

            }
        }
        else{
            mono = stereo;
        }
    }
    else{
        mono = stereo;
    }
    return mono;
}

WavHeader::WavHeader()
{
}

WavHeader::WavHeader(unsigned long sample_rate, unsigned short bits_per_sample, unsigned short channels, unsigned long data_size)
{
    memset(this, 0, 44);

    // Set Riff-Chunk
    memcpy(this->riff_id, "RIFF", 4);
    this->riff_size = data_size + 44;
    memcpy(this->riff_format, "WAVE", 4);

    // Set Fmt-Chunk
    memcpy(this->fmt_id, "fmt ", 4);
    this->fmt_size = 16;
    this->fmt_format = 1; //WAVE_FORMAT_PCM = 1
    this->fmt_channels = channels;
    this->fmt_sample_rate = sample_rate;
    this->fmt_block_align = channels*bits_per_sample/8;
    this->fmt_byte_rate = channels*bits_per_sample*sample_rate/8;
    this->fmt_bits_per_sample = bits_per_sample;

    // Set Data-Chunk
    memcpy(this->data_id, "data", 4);
    this->data_size = data_size;
}

WavHeader::WavHeader(QByteArray ba)
{
    QDataStream ds(&ba, QIODevice::ReadOnly);
    ds.setByteOrder(QDataStream::LittleEndian);

    ds.readRawData((char *)riff_id, 4);
    ds >> riff_size;
    ds.readRawData((char *)riff_format, 4);

    ds.readRawData((char *)fmt_id, 4);
    ds >> fmt_size;
    ds >> fmt_format;
    ds >> fmt_channels;
    ds >> fmt_sample_rate;
    ds >> fmt_byte_rate;
    ds >> fmt_block_align;
    ds >> fmt_bits_per_sample;

    ds.readRawData((char *)data_id, 4);
    ds >> data_size;
}

QByteArray WavHeader::getByteArray()
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);

    ds.writeRawData((const char *)riff_id, 4);
    ds << riff_size;
    ds.writeRawData((const char *)riff_format, 4);

    ds.writeRawData((const char *)fmt_id, 4);
    ds << fmt_size;
    ds << fmt_format;
    ds << fmt_channels;
    ds << fmt_sample_rate;
    ds << fmt_byte_rate;
    ds << fmt_block_align;
    ds << fmt_bits_per_sample;

    ds.writeRawData((const char *)data_id, 4);
    ds << data_size;

    return ba;
}

ALenum WavHeader::getALFormat()
{
    ALenum format = 0;
    if(this->fmt_bits_per_sample == 8)
    {
        if(this->fmt_channels == 1)
            format = AL_FORMAT_MONO8;
        else if(this->fmt_channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if(this->fmt_bits_per_sample == 16)
    {
        if(this->fmt_channels == 1)
            format = AL_FORMAT_MONO16;
        else if(this->fmt_channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    if(format == 0)
    {
        qDebug() << "Error: Incompatible WAV format (" << this->fmt_bits_per_sample << "," << this->fmt_channels << "," << this->fmt_size << "," << this->fmt_sample_rate << ")";
        return AL_FORMAT_MONO16;
    }    
    return format;
}
