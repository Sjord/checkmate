/* sort functions */

#include "mp3checker.h"
#include <tchar.h>

/* puts directories on top */
static int DirSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	if ((fi1->filetype & FILE_DIRECTORY)&&(!(fi2->filetype & FILE_DIRECTORY))) {
		return -1;
	}
	if ((fi2->filetype & FILE_DIRECTORY)&&(!(fi1->filetype & FILE_DIRECTORY))) {
		return 1;
	}
	return 0;
}

/* puts scanned files at the bottom */
static int InfoSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	if ((fi1->info==NULL)&&(fi2->info!=NULL)) {
		return -1;
	}
	if ((fi2->info==NULL)&&(fi1->info!=NULL)) {
		return 1;
	}
	return 0;
}

static int NameSort(const TCHAR * name1, const TCHAR * name2) {
	if (0==_tcscmp(name1, TEXT(".."))) return -1;
	if (0==_tcscmp(name2, TEXT(".."))) return 1;
	return _tcsicmp(name1, name2);
}

int NormalAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int dirsort=DirSort(elem1, elem2);
	if (dirsort) return dirsort;
	return NameSort(fi1->filename, fi2->filename);
}

int NormalDescendingSort(const void * elem1, const void * elem2) {
	return -NormalAscendingSort(elem1, elem2);
}

int SamplerateAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->samplerate-fi2->info->samplerate);
		if (sort) return sort;
	}
	return NameSort(fi1->filename, fi2->filename);
}

int SamplerateDescendingSort(const void * elem1, const void * elem2) {
	return -SamplerateAscendingSort(elem1, elem2);
}

int BitrateAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->bitrate-fi2->info->bitrate);
		if (sort) return sort;
	}
	return NameSort(fi1->filename, fi2->filename);
}

int BitrateDescendingSort(const void * elem1, const void * elem2) {
	return -BitrateAscendingSort(elem1, elem2);
}

int FramesAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->frames-fi2->info->frames);
		if (sort) return sort;
	}
	return NameSort(fi1->filename, fi2->filename);
}

int FramesDescendingSort(const void * elem1, const void * elem2) {
	return -FramesAscendingSort(elem1, elem2);
}

int TimeAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->time-fi2->info->time);
		if (sort) {
			return sort;
		}
	}
	return NameSort(fi1->filename, fi2->filename);
}

int TimeDescendingSort(const void * elem1, const void * elem2) {
	return -TimeAscendingSort(elem1, elem2);
}

int LayerAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->layer-fi2->info->layer);
		if (sort) {
			return sort;
		}
	}
	return NameSort(fi1->filename, fi2->filename);
	
}

int LayerDescendingSort(const void * elem1, const void * elem2) {
	return -LayerAscendingSort(elem1, elem2);
}

int VersionAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->version-fi2->info->version);
		if (sort) {
			return sort;
		}
	}
	return NameSort(fi1->filename, fi2->filename);
}

int VersionDescendingSort(const void * elem1, const void * elem2) {
	return -VersionAscendingSort(elem1, elem2);
}

int SizeAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	sort=(fi1->filesize-fi2->filesize);
	if (sort) return sort;
	return NameSort(fi1->filename, fi2->filename);
}

int SizeDescendingSort(const void * elem1, const void * elem2) {
	return -SizeAscendingSort(elem1, elem2);
}

int ResultAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);
	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		if (fi1->filetype & FILE_DIRECTORY) {
			sort=(fi1->dirinfo->cIsMP3-fi2->dirinfo->cIsMP3);
			if (sort) return sort;
			sort=(fi1->dirinfo->cGood-fi2->dirinfo->cGood);
			if (sort) return sort;
		} else {
			sort=(fi2->info->ismp3file-fi1->info->ismp3file);
			if (sort) return sort;
			sort=(fi1->info->errors-fi2->info->errors);
			if (sort) return sort;
		}
	}
	return NameSort(fi1->filename, fi2->filename);
}

int ResultDescendingSort(const void * elem1, const void * elem2) {
	return -ResultAscendingSort(elem1, elem2);
}

int VBRAscendingSort(const void * elem1, const void * elem2) {
	FileInfo * fi1=*((FileInfo **)elem1);
	FileInfo * fi2=*((FileInfo **)elem2);

	int extsort, sort;
	extsort=DirSort(elem1, elem2);
	if (extsort) return extsort;
	extsort=InfoSort(elem1, elem2);
	if (extsort) return extsort;
	if ((fi1->info!=NULL)&&(fi2->info!=NULL)) {
		sort=(fi1->info->vbr-fi2->info->vbr);
		if (sort) {
			return sort;
		}
	}
	return NameSort(fi1->filename, fi2->filename);
}

int VBRDescendingSort(const void * elem1, const void * elem2) {
	return -VBRAscendingSort(elem1, elem2);
}