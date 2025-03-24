export class State {
    public loaded: boolean = $state(false);
    public selectedBox: number | null = $state(null);
    public selectedRectangle: number | null = $state(null);

    public selectBox(index: number) {
        this.selectedRectangle = null;
        this.selectedBox = index;
    }

    public unselectBox() {
        this.selectedRectangle = null;
        this.selectedBox = null;
    }

    public selectRectangle(index: number) {
        if (this.selectedBox === null) {
            return;
        }
        this.selectedRectangle = index;
    }

    public unselectRectangle() {
        this.selectedRectangle = null;
    }

    public onLoaded() {
        this.loaded = true;
    }
}
