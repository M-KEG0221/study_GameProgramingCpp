#include "TileMapComponent.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
	, mTileSet()
{
}

void TileMapComponent::SetTileSet(SDL_Texture* texture, const int rows, const int columns
)
{
	SetTexture(texture);
	TileSet tsd;
	tsd.mTexture = texture;
	tsd.c = columns;
	tsd.r = rows;
	SDL_QueryTexture(texture, NULL, NULL, &tsd.w, &tsd.h);
	mTileSet = tsd;
}

void TileMapComponent::Update(float deltaTime)
{
	//もしスクロールに対応させたいなら
}

void TileMapComponent::Draw(SDL_Renderer* renderer)
{
	SDL_Rect srcRect;
	//タイルは全て同じ大きさなので予め設定
	srcRect.w = static_cast<int>(mTileSet.w / mTileSet.r);
	srcRect.h = static_cast<int>(mTileSet.h / mTileSet.c);

	int rowIdx = 0;
	for (auto& tileMapRow : mTileMap)
	{
		int columnIdx = 0;
		for (auto& tile : tileMapRow)
		{
			if (tile != -1)
			{
				// 描画したいタイルマップ上のタイル位置
				srcRect.x = static_cast<int>((tile % mTileSet.r) * srcRect.w);
				srcRect.y = static_cast<int>((tile / mTileSet.r) * srcRect.h);

				SDL_Rect dstRect;
				// タイルの描画サイズ
				dstRect.w = static_cast<int>(mScreenSize.x / tileMapRow.size());
				dstRect.h = static_cast<int>(mScreenSize.y / mTileMap.size());
				// タイルの描画位置
				dstRect.x = static_cast<int>(columnIdx * dstRect.w);
				dstRect.y = static_cast<int>(rowIdx * dstRect.h);

				SDL_RenderCopy(renderer,
					mTexture,
					&srcRect,
					&dstRect
				);
			}
			columnIdx++;
		}
		rowIdx++;
	}
}

void TileMapComponent::LoadCSV(const std::string& fileName)
{
	//csvファイル読み込み
	std::ifstream tileMap(fileName);//「tileMap」は変数名

	//ファイルが見つからなかった場合のエラー処理
	if (!tileMap)
	{
		SDL_Log("TileMapComponent：csvファイルが見つかりませんでした %s", fileName.c_str());
		return;//要検討。ゲームを落とす処理がベスト？
	}

	//csvファイル1行をrowStrに入れる。それを最後の行まで繰り返す
	std::string rowStr;
	while (std::getline(tileMap, rowStr))
	{
		std::vector<int> rowTiles;
		std::string tileStr;
		std::istringstream iss(rowStr);//入力専用の文字列ストリーム

		while (std::getline(iss, tileStr, ','))
		{
			int tile = std::stoi(tileStr);//String TO Int 
			rowTiles.push_back(tile);
		}

		mTileMap.push_back(rowTiles);
	}
}