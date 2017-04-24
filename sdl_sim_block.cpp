#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <numeric>

//Screen dimension constants
const int SCREEN_WIDTH = 950;
const int SCREEN_HEIGHT = 600;
const int CELL_ROWS = 80;
const int CELL_COLS = 100;
const int CELL_SPACING = 0;
const int ROAD_WIDTH = 20;
const int MENU_WIDTH = 150;

int initSdl();
void initGameboard();
bool loadMedia();
void close();

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int cellValue(int (&cells)[CELL_ROWS][CELL_COLS], int row, int column) {
  return cells[row][column] & 0xF;
}
int cellType(int (&cells)[CELL_ROWS][CELL_COLS], int row, int column) {
  return (cells[row][column] & 0xF0) >> 4;
}
void setCellValue(int (&cells)[CELL_ROWS][CELL_COLS], int row, int column, int value) {
  cells[row][column] = (cells[row][column] & 0xF0) + value;
}
void setCellType(int (&cells)[CELL_ROWS][CELL_COLS], int row, int column, int type) {
  cells[row][column] = (cells[row][column] & 0xF) + (type << 4);
}

int initSdl()
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    return -1;
  }

  //Set texture filtering to linear
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    printf( "Warning: Linear texture filtering not enabled!" );
  }

  window = SDL_CreateWindow( "SIM BLOCK", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
  if( window == NULL )
  {
    printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
    return -1;
  }
  //Create renderer for window
  renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if( renderer == NULL )
  {
    printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
    return -1;
  }

  //Initialize renderer color
  SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );

  //Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    return -1;
  }

  return 0;
}

bool loadMedia()
{
  return 0;
}

void close()
{
  //Destroy window
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  window = NULL;
  renderer = NULL;

  //Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

int cellHeight;
int cellWidth;
int cellZeroX;
int cellZeroY;
int cells[CELL_ROWS][CELL_COLS] = {
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 2, 1, 0, 0, 0, 0},
  {0, 1, 1, 2, 2, 1, 0, 0, 0, 0},
  {1, 2, 2, 3, 2, 2, 1, 0, 0, 0},
  {1, 1, 1, 1, 2, 1, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
};
int lastCells[CELL_ROWS][CELL_COLS] = {{0}};

void initGameboard() {
  srand(time(NULL));
  int max_cell_height = (SCREEN_HEIGHT - ROAD_WIDTH*2 - CELL_SPACING*(CELL_ROWS + 1)) / CELL_ROWS;
  int max_cell_width = (SCREEN_WIDTH - MENU_WIDTH - ROAD_WIDTH*2 - CELL_SPACING*(CELL_COLS + 1)) / CELL_COLS;
  cellHeight = cellWidth = std::min(max_cell_height, max_cell_width);

  cellZeroY = (SCREEN_HEIGHT - CELL_SPACING*(CELL_ROWS+1) - cellHeight*CELL_ROWS) / 2;
  cellZeroX = (SCREEN_WIDTH - MENU_WIDTH - CELL_SPACING*(CELL_COLS+1) - cellWidth*CELL_COLS) / 2;
}

void drawRoads() {
  // Western Road
  SDL_Rect wRoadRect = { cellZeroX - ROAD_WIDTH - CELL_SPACING, 0, ROAD_WIDTH, SCREEN_HEIGHT};
  SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x33, 0xFF );		
  SDL_RenderFillRect( renderer, &wRoadRect );

  // Eastern Road
  SDL_Rect eRoadRect = { cellZeroX + CELL_COLS*(cellWidth + CELL_SPACING), 0, ROAD_WIDTH, SCREEN_HEIGHT};
  SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x33, 0xFF );		
  SDL_RenderFillRect( renderer, &eRoadRect );

  // Northern Road
  SDL_Rect nRoadRect = { 0, cellZeroY - ROAD_WIDTH - CELL_SPACING, SCREEN_WIDTH, ROAD_WIDTH};
  SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x33, 0xFF );		
  SDL_RenderFillRect( renderer, &nRoadRect );

  // Southern Road
  SDL_Rect sRoadRect = { 0, cellZeroY + CELL_ROWS*(cellHeight + CELL_SPACING), SCREEN_WIDTH, ROAD_WIDTH};
  SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x33, 0xFF );		
  SDL_RenderFillRect( renderer, &sRoadRect );
}

void clearScreen() {
  SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear( renderer );
}

int colors[2][5][3] = {
  {
    {0x0A, 0x5C, 0x34},
    {0x1F, 0x83, 0x35},
    {0x33, 0xAA, 0x35},
    {0x7F, 0xC5, 0x77},
    {0xCB, 0xE0, 0xB8}
  },
  {
    {0x0A, 0x34, 0x5C},
    {0x1F, 0x35, 0x83},
    {0x33, 0x35, 0xAA},
    {0x7F, 0x77, 0xC5},
    {0xCB, 0xB8, 0xE0}
  }
};
void drawCells() {
  for (int r=0; r<CELL_ROWS; r++) {
    for (int c=0; c<CELL_COLS; c++) {
      int valueIndex = cellValue(cells, r, c);
      int typeIndex = cellType(cells, r, c);
      SDL_Rect cellRect = { cellZeroX + c*(cellWidth + CELL_SPACING), cellZeroY + r*(cellHeight + CELL_SPACING), cellWidth, cellHeight};
      SDL_SetRenderDrawColor( renderer, colors[typeIndex][valueIndex][0], colors[typeIndex][valueIndex][1], colors[typeIndex][valueIndex][2], 0xFF );
      SDL_RenderFillRect( renderer, &cellRect );
    }
  }
}


enum CellValue {
  ZERO_VALUE_CELL,
  LOW_VALUE_CELL,
  MEDIUM_VALUE_CELL,
  HIGH_VALUE_CELL,
  EXTREME_VALUE_CELL,
  CellValue_MAX
};
enum CellType { UNDEVELOPED_CELL_TYPE, RESIDENTIAL_CELL_TYPE, COMMERCIAL_CELL_TYPE, INDUSTRIAL_CELL_TYPE, CellType_MAX };

int cellValueCounts[CellValue_MAX];
int cellValueCountsCount;
int currentValue;
int getNewCellValue(int (&cellMatrix)[CELL_ROWS][CELL_COLS], int row, int column) {
  std::fill(cellValueCounts, cellValueCounts+CellValue_MAX, 0);
  cellValueCountsCount = 0;
  currentValue = cellValue(cellMatrix, row, column);

  for (int r = row - 1; r <= row + 1 && r < CELL_ROWS; r++) {
    for (int c = column - 1; c <= column + 1 && c < CELL_COLS; c++) {
      if (r < 0)
        r = 0;
      if (c < 0)
        c = 0;

      cellValueCounts[cellValue(cellMatrix, r, c)] += 1;
      cellValueCountsCount += 1;
    }
  }

  // for (int i=LOW_VALUE_CELL; i<=HIGH_VALUE_CELL; i++) {
  //   if (cellValueCounts[i] == 5 && i<HIGH_VALUE_CELL) {
  //     return i + 1;
  //   }
  //   else if (cellValueCounts[i] == 7) {
  //     return i - 1;
  //   }
  // }

  int roll = rand() % 200 + 1;
  int newTargetValue = -1;

  int matchIfOver = (200 - cellValueCountsCount);
  for (int i=0; i<CellValue_MAX; i++) {
    if (roll > matchIfOver && roll <= matchIfOver + cellValueCounts[i]) {
      newTargetValue = i;
    }
    matchIfOver += cellValueCounts[i];
  }


  if (newTargetValue == -1) {
    return currentValue;
  }
  else if (newTargetValue > currentValue && rand() % 100 < 100) {
    return currentValue + 1;
  }
  else if (newTargetValue < currentValue && rand() % 100 < 80) {
    return currentValue - 1;
  }
  return currentValue;
}

void updateCells() {
  std::copy(&cells[0][0], &cells[0][0] + CELL_ROWS*CELL_COLS, &lastCells[0][0]);
  for (int r=0; r<CELL_ROWS; r++) {
    for (int c=0; c<CELL_COLS; c++) {
      setCellValue(cells, r, c, getNewCellValue(lastCells, r, c));
    }
  }
}

int ticks = 0;
void maybeUpdateCells() {
  if (ticks % 2 == 0)
    updateCells();
  ticks += 1;
}

bool mouseIsDown = false;
void handleEvent( SDL_Event* e )
{
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    mouseIsDown = true;
  }
  if (e->type == SDL_MOUSEMOTION) {
    if (mouseIsDown) {
      int x, y;
      SDL_GetMouseState(&x, &y);

      // See if it was in a cell
      bool isInCellsBoxX = x >= cellZeroX && x <= cellZeroX + (cellWidth + CELL_SPACING)*CELL_COLS;
      bool isInCellsBoxY = y >= cellZeroY && y <= cellZeroY + (cellHeight + CELL_SPACING)*CELL_ROWS;
      if (isInCellsBoxX && isInCellsBoxY) {
        int col = (x - cellZeroX) / (cellWidth + CELL_SPACING);
        int row = (y - cellZeroY) / (cellHeight + CELL_SPACING);
        setCellValue(cells, row, col, EXTREME_VALUE_CELL);
        setCellType(cells, row, col, 1);
      }
    }
  }
  else if(e->type == SDL_MOUSEBUTTONUP) {
    mouseIsDown = false;
  }
}

int main( int argc, char* args[] )
{
  if(initSdl() < 0) {
    printf( "Failed to initialize!\n" );
    return -1;
  }
  initGameboard();

  bool quit = false;
  SDL_Event e;

  // Main Loop
  while( !quit )
  {

    while( SDL_PollEvent( &e ) != 0 )
    {
      //User requests quit
      if( e.type == SDL_QUIT )
      {
        quit = true;
      }
      handleEvent(&e);
    }

    maybeUpdateCells();

    clearScreen();
    drawRoads();
    drawCells();

    //Update screen
    SDL_RenderPresent( renderer );
  }

  //Free resources and close SDL
  close();

  return 0;
}
