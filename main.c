#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

void RenderStandard(Image *image, int offset);
void RenderOldTv(Image *image, int offset);
void RenderBooting(Image *image, int offset, int render_rate);
void ImageQuantizeEuclidean(Image *image);
void ImageQuantizeManhattan(Image *image);
void ImagePixelate(Image *image, int pixel_size);
Point GetPointFromPixel(int pixel, int width);
int GetIndexFromPoint(Point point, int width);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window");

    int monitorWidth = GetMonitorWidth(1);
    int monitorHeight = GetMonitorHeight(1);
    int windowPosX = (monitorWidth / 2) - (SCREEN_WIDTH / 2);
    int windowPosY = (monitorHeight / 2) - (SCREEN_HEIGHT / 2);

    int monitorCount = GetMonitorCount();
    if (monitorCount > 1)
    {
        Vector2 monitorPosition = GetMonitorPosition(1);
        SetWindowPosition(monitorPosition.x + windowPosX, monitorPosition.y + windowPosY);
    }

    Image image = LoadImage("./goku.png");
    ImageResize(&image, 400, 600);

    ImagePixelate(&image, 16);
    ImageQuantizeManhattan(&image);

    SetTargetFPS(60);
    int offset = 100;

    // RenderBooting(&image, offset, 100);
    // RenderOldTv(&image, offset);
    RenderStandard(&image, offset);

    CloseWindow();
    return 0;

}

void RenderStandard(Image *image, int offset) {
    Texture2D texture = LoadTextureFromImage(*image);
    UnloadImage(*image);

    while (!WindowShouldClose()) {
	BeginDrawing();
	    ClearBackground(BLACK);
	    DrawTexture(texture, offset, offset, WHITE);
	EndDrawing();    
    }

    UnloadTexture(texture);
}

void RenderOldTv(Image *image, int offset) {
    Color *pixels = LoadImageColors(*image);

    while (!WindowShouldClose()) {
	BeginDrawing();
	ClearBackground(BLACK);

	    for (int i = 0; i < 100000; i++) {
		int p = rand() % (0 + (image->width * image->height));
		Point xy = GetPointFromPixel(p, image->width);
		Color color = { pixels[p].r, pixels[p].g, pixels[p].b, pixels[p].a };
		DrawPixel(xy.x+offset, xy.y+offset, color);
	    }
	EndDrawing();    
    }
    UnloadImage(*image);
}

void RenderBooting(Image *image, int offset, int render_rate) {
    Color *pixels = LoadImageColors(*image);
    int rendered = 1;
    while (!WindowShouldClose()) {
	BeginDrawing();
	    for (int i = 0; i < rendered; i++) {
		int p = rand() % (0 + (image->width * image->height));
		Point xy = GetPointFromPixel(p, image->width);
		Color color = { pixels[p].r, pixels[p].g, pixels[p].b, pixels[p].a };
		DrawPixel(xy.x+offset, xy.y+offset, color);
	    }

	    rendered = rendered + render_rate;
	EndDrawing();    
    }
    UnloadImage(*image);
}

/// Quantization performed using euclidean distance
void ImageQuantizeEuclidean(Image *image) {
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;
    Color *pixels = LoadImageColors(*image);
    Color *colors = (Color[]) {
	{121, 235, 0, 255},
	{70, 180, 33, 255},
	{30, 127, 35, 255},
	{4, 76, 26, 255},
	{0, 31, 8, 255}
    };

    for (int p = 0; p < (image->width * image->height); p++) {
	int r = pixels[p].r;
	int g = pixels[p].g;
	int b = pixels[p].b;
	int a = pixels[p].a;

	if (r != 0 && g != 0 && b != 0 && a != 0) {
	    float min_distance = -1.0;
	    int closest_index = -1;

	    for (int c = 0; c < 5; c++) {
		float r_diff = (float)(colors[c].r - r); 
		float g_diff = (float)(colors[c].g - g); 
		float b_diff = (float)(colors[c].b - b); 
		float a_diff = (float)(colors[c].a - a); 
		float distance = sqrtf((r_diff * r_diff) + (g_diff * g_diff) + (b_diff * b_diff) + (a_diff * a_diff));

		if (c == 0) {
		    closest_index = c;
		    min_distance = distance;
		    continue;
		}

		if (distance < min_distance) {
		    closest_index = c;
		    min_distance = distance;
		}
	    }
	
	    pixels[p].r = colors[closest_index].r;
	    pixels[p].g = colors[closest_index].g;
	    pixels[p].b = colors[closest_index].b;
	    pixels[p].a = colors[closest_index].a;
	}
    }

    int format = image->format;
    free(image->data);

    image->data = pixels;
    image->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    ImageFormat(image, format);
}

/// Quantization performed using Manhattan distance
void ImageQuantizeManhattan(Image *image) {
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    ImageAlphaPremultiply(image);

    Color *pixels = LoadImageColors(*image);
    Color *colors = (Color[]) {
	{121, 235, 0, 255},
	{70, 180, 33, 255},
	{30, 127, 35, 255},
	{4, 76, 26, 255},
	{0, 31, 8, 255}
    };

    for (int p = 0; p < (image->width * image->height); p++) {
	int r = pixels[p].r;
	int g = pixels[p].g;
	int b = pixels[p].b;
	int a = pixels[p].a;

	if (a != 0) {
	    int min_distance = -1;
	    int closest_index = -1;

	    for (int c = 0; c < 5; c++) {
		int r_diff = abs(colors[c].r - r); 
		int g_diff = abs(colors[c].g - g); 
		int b_diff = abs(colors[c].b - b); 
		int a_diff = abs(colors[c].a - a); 
		int distance = r_diff + g_diff + b_diff + a_diff;

		if (c == 0) {
		    closest_index = c;
		    min_distance = distance;
		    continue;
		}

		if (distance < min_distance) {
		    closest_index = c;
		    min_distance = distance;
		}
	    }

	    pixels[p].r = colors[closest_index].r;
	    pixels[p].g = colors[closest_index].g;
	    pixels[p].b = colors[closest_index].b;
	    pixels[p].a = colors[closest_index].a;
	}
    }

    free(image->data);

    image->data = pixels;
    image->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    ImageFormat(image, image->format);
}

/// Pixel Size must be square rootable with no remainder
void ImagePixelate(Image *image, int pixel_size) {
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    ImageAlphaPremultiply(image);
    Color *pixels = LoadImageColors(*image);
    int pixel_lh = sqrt(pixel_size);
    int height = image->height / pixel_lh;
    int width = image->width / pixel_lh;

    for (int b = 0; b < (width * height); b++) {
	Point point = GetPointFromPixel(b, width);
	
	// find original pixel index
	int og_x = point.x * pixel_lh;
	int og_y = point.y * pixel_lh;
	Color block_pixels[pixel_size];
	int pixel_idxs[pixel_size];

	for (int x = 0; x < pixel_lh; x++) {
	    for (int y = 0; y < pixel_lh; y++) {
		Point point = {x + og_x, y + og_y};
		int pixel_idx = GetIndexFromPoint(point, image->width);

		Point block_point = {x, y};
		int block_idx = GetIndexFromPoint(block_point, pixel_lh);

		block_pixels[block_idx] = pixels[pixel_idx];
		pixel_idxs[block_idx] = pixel_idx;
	    }
	}

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	for (int i = 0; i < pixel_size; i++) {
	    r += block_pixels[i].r;
	    g += block_pixels[i].g;
	    b += block_pixels[i].b;
	    a += block_pixels[i].a;
	}

	// avg the pixels
	Color block_rbga = {r / pixel_size, g / pixel_size, b / pixel_size , a / pixel_size}; 

	for (int i = 0; i < pixel_size; i++) {
	    int target_idx = pixel_idxs[i];
	    pixels[target_idx] = block_rbga;
	}
    }

    free(image->data);

    image->data = pixels;
    image->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    ImageFormat(image, image->format);
}

Point GetPointFromPixel(int pixel, int width) {
    Point point;
    point.x = pixel % width;
    point.y = (int)floor((float)pixel / width);

    return point;
}

int GetIndexFromPoint(Point point, int width) {
    return point.y * width + point.x;
}
