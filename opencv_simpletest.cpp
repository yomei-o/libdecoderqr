#include <stdio.h>
#include <vector>

#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif /* _WIN32 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "core.hpp"
#include "imgproc.hpp"
#include "tracking.hpp"
#include "imgproc_c.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "decodeqr.h"

using namespace std;


void changeb_g(unsigned char* p, int x, int y, int c)
{
	int ct = x*y;
	int i;
	unsigned char t;
	for (i = 0; i < ct; i++){
		t = p[0];
		p[0] = p[2];
		p[2] = t;
		p[3] = 255;
		p += c;
	}
}


int main(int argc,char *argv[])
{


	unsigned char*p;
	int x = -1, y = -1;
	int n;

#if defined(_WIN32) && !defined(__GNUC__)
	//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//load image
	p = stbi_load("01-1.jpg", &x, &y, &n, 4);
	if (p == NULL || x < 1 || y < 1)return 1;

	// R<-->B
	changeb_g(p, x, y, 4);

	cv::Mat color = cv::Mat(y, x, CV_8UC4);

	//copy data
	memcpy(color.data, p, x * 4 * y);
	stbi_image_free(p);

	
	IplImage src = color;



    //
    // show version info
    //
    printf("libdecodeqr version %s\n",qr_decoder_version());

    //
    // initialize
    //
    QrDecoderHandle decoder=qr_decoder_open();


	//
    // do decode using default parameter
    //
    short stat=qr_decoder_decode_image(decoder,&src);
    printf("STATUS=%04x\n",stat);

	//
    // get QR code header
    //
    QrCodeHeader header;
    if(qr_decoder_get_header(decoder,&header)){
        //
        // get QR code text
        // To null terminate, a buffer size is larger than body size.
        //
        char *buf=new char[header.byte_size+1];
        qr_decoder_get_body(decoder,(unsigned char *)buf,header.byte_size+1);
        printf("%s\n",buf);
		delete[] buf;
    }

    //
    // finalize
    //
    qr_decoder_close(decoder);


    return(0);
}
