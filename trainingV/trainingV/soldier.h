class Soldier{
	private:
		int x1, x2, y1, y2;
		int type;
		int th;
		int p;
		double e;
		int getAreaSum(IntImg *intImg,int x1,int y1,int x2,int y2){
			int sum=0;
			sum+=intImg->data[y2-1][x2-1];
			if(y1>0)
				sum-=intImg->data[y1-1][x2-1];
			if(x1>0)
				sum-=intImg->data[y2-1][x1-1];
			if(y1>0 && x1>0)
				sum+=intImg->data[y1-1][x1-1];
			return sum;
		};
	public:
		
		Soldier(int x1,int y1,int x2,int y2,int type,int p,int th){	
			this->x1=x1;
			this->x2=x2;
			this->y1=y1;
			this->y2=y2;
			this->type=type;
			this->p=p;
			this->th=th;
		};
		void setAll(int xx1,int xx2,int yy1,int yy2, int ttype, int tth, int pp ,double ee){
			x1 = xx1;
			x2 = xx2;
			y1 = yy1;
			y2 = yy2;
			type = ttype;
			th = tth;
			p = pp;
			e = ee;
		};
		int getX1(){
			return x1;
		};
		int getX2(){
			return x2;
		};
		int getY1(){
			return y1;
		};
		int getY2(){
			return y2;
		};
		int getType(){
			return type;
		};
		int getP(){
			return p;
		};
		double getE(){
			return e;
		};
		int getth(){
			return th;
		};
		void setP(int in){
			p = in;
		};
		void setTh(int in){
			th = in;
		};
		void setE(double in){
			e = in;
		};
		void setth(int t){
			th= t;
		};
		

		int judge(IntImg *intImg){
			int sum=comput(intImg);
			
			if(p==1)
				return (int)th>=sum;
			else
				return (int)th<=sum;
		};
		
		int comput(IntImg *intImg){
			switch(type){
				case 0:
					return getAreaSum(intImg,x1,y1,(x2+x1)/2,y2)-getAreaSum(intImg,(x2+x1)/2,y1,x2,y2);
					break;
				case 1:
					return getAreaSum(intImg,x1,y1,x2,(y1+y2)/2)-getAreaSum(intImg,x1,(y1+y2)/2,x2,y2);
					break;
				case 2:
					return getAreaSum(intImg,x1,y1,x1+(x2-x1)/3,y2)-getAreaSum(intImg,x1+(x2-x1)/3,y1,x1+(x2-x1)*2/3,y2)+getAreaSum(intImg,x1+(x2-x1)*2/3,y1,x2,y2);
					break;
				case 3:
					return getAreaSum(intImg,x1,y1,x2,y1+(y2-y1)/3)-getAreaSum(intImg,x1,y1+(y2-y1)/3,x2,y1+(y2-y1)*2/3)+getAreaSum(intImg,x1,y1+(y2-y1)*2/3,x2,y2);
					break;
				case 4:
					return getAreaSum(intImg,x1,y1,(x2+x1)/2,(y1+y2)/2)+getAreaSum(intImg,(x2+x1)/2,(y1+y2)/2,x2,y2)-getAreaSum(intImg,(x2+x1)/2,y1,x2,(y1+y2)/2)-getAreaSum(intImg,x1,(y1+y2)/2,(x2+x1)/2,y2);
					break;
			}
		};
		
		void getData(char *str){
			sprintf_s(str,200,"%02d %02d %02d %02d %d %2d %8d %E",x1,y1,x2,y2,type,p,th,e);
			//sprintf_s(str, 200, "%02d %02d %02d %02d %d %2d", x1, y1, x2, y2, type,p);
		}
};
